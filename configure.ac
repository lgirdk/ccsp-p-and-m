##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2015 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
#                                              -*- Autoconf -*-
# Process this file with autoconf to produce configure script.
#

AC_PREREQ([2.65])
AC_INIT([tr181], [1.0], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
LT_INIT

AC_ARG_ENABLE([unitTestDockerSupport],
             AS_HELP_STRING([--enable-unitTestDockerSupport],[enable docker support for unit testing (default is no)]),
             [
              UNIT_TEST_DOCKER_SUPPORT=""
               case "${enableval}" in
                yes) UNIT_TEST_DOCKER_SUPPORT=true
                     m4_if(m4_sysval,[0],[AC_CONFIG_FILES([source/test/Makefile])]);;
                no) UNIT_TEST_DOCKER_SUPPORT=false;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-unitTestDockerSupport ]);;
               esac
             ],
             [echo "Docker support is disabled"])
AM_CONDITIONAL([UNIT_TEST_DOCKER_SUPPORT], [test x$UNIT_TEST_DOCKER_SUPPORT = xtrue])

AC_PREFIX_DEFAULT(`pwd`)
AC_ENABLE_SHARED
AC_DISABLE_STATIC

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_LIBTOOL(libtool)

AC_ARG_ENABLE([maptsupport],
             AS_HELP_STRING([--enable-maptsupport],[enable mapt support (default is no)]),
             [
              case "${enableval}" in
               yes) MAPT_SUPPORT_ENABLED=true;;
               no)  MAPT_SUPPORT_ENABLED=false;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-maptsupport ]);;
              esac
             ],
             [echo "mapt is disabled"])
AM_CONDITIONAL(FEATURE_SUPPORT_MAPT_NAT46, test "x$MAPT_SUPPORT_ENABLED" = xtrue)

SYSTEMD_CFLAGS=" "
SYSTEMD_LDFLAGS=" "
AC_ARG_ENABLE([notify],
	AS_HELP_STRING([--enable-notify],[enable systemd notify]),
	[
	  case "${enableval}" in
  	   yes) SYSTEMD_CFLAGS="-DENABLE_SD_NOTIFY"
                SYSTEMD_LDFLAGS="-lsystemd" ;;
           no) AC_MSG_ERROR([systemd notify is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-notify ]) ;;
           esac
           ],
	 [echo "headless build is enable"])

AC_ARG_ENABLE([wifimanagesupport],
             AS_HELP_STRING([--enable-wifimanagesupport],[enable wifi manage support (default is no)]),
             [
              case "${enableval}" in
               yes) WIFI_MANAGE_SUPPORT_ENABLED=true;;
               no)  WIFI_MANAGE_SUPPORT_ENABLED=false;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-wifimanagesupport ]);;
              esac
             ],
             [echo "wifimanage is disabled"])
AM_CONDITIONAL(FEATURE_SUPPORT_MANAGE_WIFI, test "x$WIFI_MANAGE_SUPPORT_ENABLED" = xtrue)

AC_ARG_ENABLE([speedboost],
             AS_HELP_STRING([--enable-speedboost],[enable speed boost support (default is no)]),
             [
              case "${enableval}" in
               yes) SPEED_BOOST_SUPPORT_ENABLED=true;;
               no)  SPEED_BOOST_SUPPORT_ENABLED=false;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-speedboost]);;
              esac
             ],
             [echo "speedboost is disabled"])
AM_CONDITIONAL(FEATURE_SUPPORT_SPEED_BOOST, test "x$SPEED_BOOST_SUPPORT_ENABLED" = xtrue)

AC_ARG_ENABLE([resourceoptimization],
             AS_HELP_STRING([--enable-resourceoptimization],[enable resource optimization (default is no)]),
             [
              case "${enableval}" in
               yes) RESOURCE_OPTIMIZATION_ENABLED=true;;
               no)  RESOURCE_OPTIMIZATION_ENABLED=false;;
               *) AC_MSG_ERROR([bad value ${enableval} for --enable-resourceoptimization]);;
              esac
             ],
             [echo "resourceoptimization is disabled"])
AM_CONDITIONAL(FEATURE_RESOURCE_OPTIMIZATION, test "x$RESOURCE_OPTIMIZATION_ENABLED" = xtrue)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_SUBST(SYSTEMD_CFLAGS)
AC_SUBST(SYSTEMD_LDFLAGS)

AC_CONFIG_FILES(
	custom/Makefile
	custom/comcast/Makefile
	custom/comcast/source/Makefile
	custom/comcast/source/TR-181/Makefile
	custom/comcast/source/TR-181/custom_ml/Makefile
	custom/comcast/source/TR-181/custom_sbapi/Makefile
	source-arm/TR-181/board_sbapi/Makefile
	source-arm/TR-181/board_ml/Makefile
	source-arm/TR-181/Makefile
	source-arm/Makefile
	source/TR-181/middle_layer_src/Makefile
	source/TR-181/integration_src.shared/Makefile
	source/TR-181/Makefile
	source/PandMSsp/Makefile
	source/Makefile
	Makefile
)

AC_SUBST(UNIT_TEST_DOCKER_SUPPORT)
AC_OUTPUT

