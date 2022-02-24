/*****************************************************************************
 * Copyright 2021 Liberty Global B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

#ifndef _COSA_SSAM_APIS_H
#define _COSA_SSAM_APIS_H

#define SSAM_PARTITION      "/var/sam"
#define SSAM_LOADER         "/var/sam_loader"
#define SSAM_PID_FILE       "/var/tmp/.sam.pid"
#define SSAM_LOCK           "/var/tmp/ssam_lock"
#define SSAM_ENV            "/var/tmp/environment"

void ssam_start (void);
void ssam_stop (void);

int CosaSetAgentpassword(const char *password);
void CosaGetAgentpassword(char **output, unsigned int *output_size);

#endif
