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
 *      Databay UDP Tmote Sky App configuration file
 * \author
 *      Leo Soares <leo@databay.dev>
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

// Contiki configs
// log color adds around 520 Bytes to rom size
#define LOG_CONF_WITH_COLOR 0
// logs from NONE TO DBG adds around 620 Bytes to rom size
#define LOG_LEVEL_APP LOG_LEVEL_DBG
// misc rom size reduction
#define UIP_CONF_TCP 0
#define PROCESS_CONF_NO_PROCESS_NAMES 1

// App configs
#define APP_DEFAULT_PUBLISH_INTERVAL 30
#define DATABAY_PROXY_UDP_SERVER_PORT 5678
#define UDP_MSG_BUFF_LEN 90

// Disable printing, can only be enabled if logs are set to
// LOG_LEVEL_NONE, or else there is no rom space
#define DATABAY_DISABLE_PRINT 1

#endif /* PROJECT_CONF_H_ */
