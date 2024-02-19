#!/usr/bin/env python3
"""
databay_udp_proxy
"""

import sys
import os
import socketserver
import socket
import threading
import argparse
import asyncio
import aiocoap
import umsgpack

DATABAY_COAP_SERVER_HOST = "app.databay.dev"
DATABAY_COAP_SERVER_PORT = "5683"
DATABAY_PROJECT_ID = os.getenv("DATABAY_PROJECT_ID") or sys.exit(
    "Error: environment variable 'DATABAY_PROJECT_ID' must be set."
)
DATABAY_API_KEY = os.getenv("DATABAY_API_KEY") or sys.exit(
    "Error: environment variable 'DATABAY_API_KEY' must be set."
)
DATABAY_SERVER_URI = f"coap://{DATABAY_COAP_SERVER_HOST}:{DATABAY_COAP_SERVER_PORT}"
DATABAY_BASE_URI = f"{DATABAY_SERVER_URI}/v1/{DATABAY_PROJECT_ID}"

parser = argparse.ArgumentParser(
    prog="databay_udp_proxy",
    description="Proxy server for databay messages in UDP packets.",
)
parser.add_argument(
    "-v", "--verbose", action="store_true", help="enable verbose output"
)
args = parser.parse_args()

verbose_flag = args.verbose


def ipv6_str_to_databay_device_id(ipv6_str):
    """Convert macaddr in ipv6 addr to databay device_id"""
    # based of:
    # https://stackoverflow.com/questions/37140846/how-to-convert-ipv6-link-local-address-to-mac-address-in-python

    # remove subnet info if given
    subnet_index = ipv6_str.find("/")
    if subnet_index != -1:
        ipv6_str = ipv6_str[:subnet_index]

    ipv6_parts = ipv6_str.split(":")
    macaddr_parts = []
    for ipv6_part in ipv6_parts[-4:]:
        while len(ipv6_part) < 4:
            ipv6_part = "0" + ipv6_part
        macaddr_parts.append(ipv6_part[:2])
        macaddr_parts.append(ipv6_part[-2:])

    # modify parts to match MAC value
    macaddr_parts[0] = f"{int(macaddr_parts[0], 16) ^ 2:02x}"
    del macaddr_parts[4]
    del macaddr_parts[3]

    return "".join(macaddr_parts)


def print_info(*args, **kwargs):
    print(*args, **kwargs)
    print("", end="", flush=True)


def print_verbose(*args, **kwargs):
    if verbose_flag:
        # Add your extra functionality here
        print(*args, **kwargs)
        print("", end="", flush=True)
    else:
        pass


def databay_publish_blocking(req_id, device_id, payload):
    async def inner():
        nonlocal device_id, payload
        uri = f"{DATABAY_BASE_URI}/{device_id}?{DATABAY_API_KEY}"
        context = await aiocoap.Context.create_client_context()

        request = aiocoap.Message(
            code=aiocoap.Code.POST,
            uri=uri,
            payload=payload,
        )
        response = await context.request(request).response

        if response.code == aiocoap.Code.CONTENT:
            # Return the response payload
            return response.payload
        else:
            coap_code_str = response.code.name_printable
            print(
                f"[{req_id}] ERROR: Failed publishing data to databay! CoAP code='{coap_code_str}'",
                end="",
                file=sys.stderr,
            )
            response_payload_hex = "".join(f"{x:02x}" for x in response.payload)
            print_verbose(
                f" payload='{response_payload_hex}'",
                end="",
                file=sys.stderr,
            )
            print("", file=sys.stderr, flush=True)
            return None

    return asyncio.run(inner())


def hijack_float_values(data):
    """converts int to float and computes values from certain low spec platforms."""
    unpacked_data = umsgpack.unpackb(data)
    schema_id = unpacked_data[0]
    attributes = unpacked_data[1]
    # check schema id
    if schema_id == 1002:  # Tmote Sky
        if isinstance(attributes, dict):
            attr_keys = attributes.keys()
            if 3 in attr_keys:
                # Temperature in Celsius (t in 14 bits resolution at 3 Volts)
                # T = -39.60 + 0.01*t
                raw_int_val = attributes[3]
                attributes[3] = (0.01 * raw_int_val) - 39.60
            if 4 in attr_keys:
                # Relative Humidity in percent (h in 12 bits resolution)
                # RH = -4 + 0.0405*h - 2.8e-6*(h*h)
                raw_int_val = attributes[4]
                attributes[4] = ((0.0405 * raw_int_val) - 4) + (
                    (-2.8 * 0.000001) * (pow(raw_int_val, 2))
                )
            if 6 in attr_keys:
                # taken from:
                # https://anrg.usc.edu/contiki/index.php/Build_your_own_application_on_Contiki
                raw_int_val = attributes[6]
                attributes[6] = raw_int_val * 0.4071

    hijacked_data = [schema_id, attributes]

    return umsgpack.packb(hijacked_data)


class DatabayProxyUDPHandler(socketserver.BaseRequestHandler):
    def handle(self):
        udp_data = self.request[0].strip()
        target_socket = self.request[1]
        current_thread = threading.current_thread()
        device_id = ipv6_str_to_databay_device_id(self.client_address[0])
        # extract thread nr by using `current_thread.name` string
        req_id = current_thread.name.split("-")[1].split(" ")[0]
        udp_data_hex = "".join(f"{x:02x}" for x in udp_data)
        print_verbose(
            f"[{req_id}] Received from '{self.client_address[0]}' with payload: '{udp_data_hex}'"
        )
        # HACK: hijack data
        hijacked_data = hijack_float_values(udp_data)

        res_data = databay_publish_blocking(req_id, device_id, hijacked_data)
        print_info(f"[{req_id}] Published data from '{device_id}' to databay.")
        if res_data:
            res_data_hex = "".join(f"{x:02x}" for x in res_data)
            print_verbose(
                f"[{req_id}] Sending to '{self.client_address[0]}' with payload: '{res_data_hex}'"
            )
            target_socket.sendto(res_data, self.client_address)
            print_info(f"[{req_id}] Sent reply from databay to '{device_id}'.")


class DatabayProxyUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    # Override address_family to listen on IPV6
    address_family = socket.AF_INET6


if __name__ == "__main__":
    HOST, PORT = "::", 5678

    server = DatabayProxyUDPServer((HOST, PORT), DatabayProxyUDPHandler)
    print_info(f"Server listening on host={HOST} port={PORT}")
    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    try:
        server.serve_forever()
    except (KeyboardInterrupt, SystemExit):
        print_info("Exiting...")
        server.shutdown()
        server.server_close()
        exit()
