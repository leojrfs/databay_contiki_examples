/*
 * Copyright (c) 2024, databay.dev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *      Databay CoAP Example configuration file.
 * \author
 *      Leo Soares <leo@databay.dev>
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
#include "databay_user_conf.h"
// Contiki configs
#define LOG_CONF_WITH_COLOR 1
#define LOG_LEVEL_APP LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_COAP LOG_LEVEL_ERR

// App configs
#define COAP_SERVER_ENDPOINT "coap://[fd00::1]"
// leading and ending slashes get cropped automatically when setting the Uri-Path
#define COAP_SERVER_URI_BASE_PATH "v1/" DATABAY_PROJECT_ID "/"
#define COAP_SERVER_URI_MAX_LEN 200

#define APP_DEFAULT_PUBLISH_INTERVAL 30
#endif /* PROJECT_CONF_H_ */
