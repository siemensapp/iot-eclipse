					MindConnect Library (MCL) v@PROJECT_VERSION@
					==================================

## Introduction
This is the code documentation for MindConnect Library. Please accept this
as an introductory document and check doxygen documentation for details on MCL.
The information on how to obtain doxygen documentation will be provided
in the following sections.

## What is it?
The MindConnect Library is a collection of functionalities which allows a
client application to connect and exchange data with the MindSphere.
The connection to the MindSphere is secured by using SSL/TLS in order to
protect client's transferred data.

## Requirements
-   In order to build the MCL you need first of all the cross make tool
	"cmake (3.5.2 and newer)".

-   The current version of the MCL works only (if not replaced by your
	implementation) together with Libcurl (@CURL_VERSION_REQUIRED@) and OpenSSL (@OPENSSL_VERSION_NUMBER_REQUIRED@@OPENSSL_VERSION_LETTER_REQUIRED@).
	Other versions of the mentioned third party libraries are not tested.

	Please make sure, that your Libcurl installation is configured to use
	OpenSSL as the TLS v1.2 implementation.

	To use OpenSSL with your implementation one of following ciphers must be
	supported and configured for SSL handshaking:

			DHE-DSS-AES128-GCM-SHA256
			DHE-DSS-AES256-GCM-SHA384
			DHE-RSA-AES128-GCM-SHA256
			DHE-RSA-AES256-GCM-SHA384
			DHE-RSA-CHACHA20-POLY1305
			ECDHE-ECDSA-AES128-GCM-SHA256
			ECDHE-ECDSA-AES256-GCM-SHA384
			ECDHE-RSA-AES128-GCM-SHA256
			ECDHE-RSA-AES256-GCM-SHA384
			ECDHE-ECDSA-CHACHA20-POLY1305
			ECDHE-RSA-CHACHA20-POLY1305

	In order to verify the correctness of the MindSphere's certificate
	please enable host and peer verification.

-   If you want to have unit and integration test executables also be build
	with cmake, please ensure that ruby is installed and available on
	the path. If you dont have ruby on your path no test executables will
	be generated.

## Preparing and Building
Building the MCL from source code is a simple task. In following steps we
will guide you through each build process and also show you how to
import and use the MCL inside your client application.

### Creating a build directory
First you need to create a working directory where cmake can generate
required files. We will use CMAKE_BUILD_DIR (e.g. ~/cmake_build_linux or
C:\\Temp\\cmake_build_windows, ...) as a replacement for the build folder.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		mkdir \<CMAKE_BUILD_DIR\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Change your current directory to the new created folder

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cd \<CMAKE_BUILD_DIR\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All next build steps are done from now on under this working directory
and source files of the MCL are not effected by any operation in those
steps. Output files from the build are located under

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		\<CMAKE_BUILD_DIR\>\\build\\\<CMAKE_BUILD_TYPE\>\\
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Prepare build folder with cmake
If you want only to build MCL project from console just enter
the following cmake command inside \<CMAKE_BUILD_DIR\>. We assume
MCL_PROJECT_FOLDER points to location of MCL's source code folder
with the top level CMakeLists.txt file.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake <MCL_PROJECT_FOLDER>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For creating the build files, cmake will look within the environment
for a suitable compiler and linker. Please keep in mind, that for a
build under windows with Visual Studio's toolchain, you need to run
vcvarsall.bat (located in your VS installation) in order to set all
environment variables.

### Selecting a cmake generator (optional)
Depending on which IDE you want to use for development you can let cmake create
project and makefiles for the MCL. Please use the help documentation of cmake
to see the list of available generators depending on your environment.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake --help
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You should see an output at the end which may look like:

The following generators are available on this platform:
	Visual Studio 14 2015 [arch] = Generates Visual Studio 2015 project
						files. Optional [arch] can be "Win64" or "ARM".
	...
	Unix Makefiles              = Generates standard UNIX makefiles.
	...
	Eclipse CDT4 - Unix Makefiles

Each of this generators can be used to create project and makefiles in
current build directory \<CMAKE_BUILD_DIR\>.

If you want to use one of the above generators just change the
cmake call to:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake -G "name of the generator" \<MCL_PROJECT_FOLDER\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake -G "Unix Makefiles" \<MCL_PROJECT_FOLDER\>
		cmake -G "Visual Studio 14 2015 Win64" \<MCL_PROJECT_FOLDER\>
		cmake -G "Eclipse CDT4 - Unix Makefiles" \<MCL_PROJECT_FOLDER\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you choose the generator for Visual Studio or Eclipse then you
will see that cmake also generates project files (like solutions for VS)
which can be opened by the according IDE.

If you want to have an output for a specific build type use the parameter
CMAKE_BUILD_TYPE. As an example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=<Debug|Release|..> \<MCL_PROJECT_FOLDER\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Specifing MCL options for build process
There are other options you can enable or disable in order to modify how MCL is build.
Each option is provided to cmake with:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake -D\<MCL_OPTION\>[=\<OPTION_VALUE\>] \<MCL_PROJECT_FOLDER\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the MCL_OPTION has no value you can skip the assigment in above command.

Allowed options can be listed as following:

		- MCL_STATICLIB :		If set to ON MCL is build as static library.
									If set to OFF (Default) MCL is build as dynamic library.

		- MCL_USE_LIBCURL:		If set to ON (Default) MCL is build with libcurl.
									If set to OFF MCL is build without libcurl. In this case
									user has to provide a module which implements an HTTPS client.
									Currenlty user must be sure libcurl is available in path environment.

		- MCL_USE_OPENSSL:  	If set to ON (Default) MCL is build with OpenSSL.
									If set to OFF MCL is build without OpenSSL. In this case
									user has to provide a module which implements the security
									layer (SHA256, MD5 calculation etc.) of MCL.
									Please make sure OpenSSL @OPENSSL_VERSION_NUMBER_REQUIRED@@OPENSSL_VERSION_LETTER_REQUIRED@ is available in path.

		- MCL_TESTING:			If set to ON (Default) and if ruby is found in path MCL
									is build with tests.
									If set to OFF MCL is build without tests.

		- MCL_CREATE_DOXYGEN:	If set to ON (Default) and if doxygen is found in path MCL
									is build with target "mcl_doc" for generating doxygen documentation.
									If set to OFF MCL is build without target "mcl_doc".

		- MCL_LOG_UTIL_LEVEL:	The value of this option is used to set log level for compilation.
									Possible values from lowest to highest level are:

									MCL_LOG_UTIL_LEVEL_VERBOSE
									MCL_LOG_UTIL_LEVEL_DEBUG
									MCL_LOG_UTIL_LEVEL_INFO
									MCL_LOG_UTIL_LEVEL_WARN
									MCL_LOG_UTIL_LEVEL_ERROR
									MCL_LOG_UTIL_LEVEL_FATAL
									MCL_LOG_UTIL_LEVEL_NONE

### Invoke cross build with cmake (optional)
It is also possible to let cmake cross build the MCL for a different
target system. For this, you need to support the required toolchain and
to create a toolchain setup file. An example for building with a
toolchain setup file you have to call cmake like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
			cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-i586-poky-linux.cmake \<MCL_PROJECT_FOLDER\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Don't forget to add any required system root folders to your path, so
cmake can find any includes for the target system. The content of the
above Toolchain-i586-poky-linux.cmake may look like (example is for
the build of MCL for an IoT2040 device with Intel's toolchain
on a debian os):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmake}
		SET(CMAKE_SYSTEM_NAME Linux)
		SET(CMAKE_SYSROOT ~/iss-iot-linux/devkit-x86/sysroots/i586-poky-linux)

		SET(CMAKE_C_COMPILER i586-poky-linux-gcc)

		SET(tools ~/iss-iot-linux/devkit-x86/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux)
		SET(CMAKE_FIND_ROOT_PATH ${tools})

		SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
		SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Another example for building MCL for embedded devices may look like:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmake}
		SET(CMAKE_SYSTEM_NAME Generic)

		INCLUDE(CMakeForceCompiler)
		CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
		CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
		SET(CMAKE_C_FLAGS "-Wall -ffunction-sections -fdata-sections -fno-builtin-memcpy -gdwarf-2 -mthumb-interwork -mcpu=arm926ej-s  -O0 -g3 -c -MMD -MP")
		SET(CMAKE_EXE_LINKER_FLAGS "-nostdlib -nostartfiles -Wl,--gc-sections -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free -Wl,--wrap=_malloc_r -Wl,--wrap=_calloc_r -Wl,--wrap=_realloc_r -Wl,--wrap=_free_r -Wl,--defsym -Wl,PAGE_SIZE=0 -Wl,--start-group -lgcc -lc -lm -lstdc++ -Wl,--end-group")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Invoke build with cmake
If the build folder preparation succeeds in previous steps now you
should be able to build the MCL project. If the command "make" e.g. is
in your toolchain then you could call it in your build folder
directly, but for other toolchain setup you may want to
call (builds all targets):

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		cmake --build .
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is also possible to choose one of the targets to be built instead of all:

	- mc        = target for building the MCL

	- mcl_doc   = target for building the Doxygen documentation of the
					MCL project

	- package   = target for generating the distribution package of MCL

	- test_\<module_name\> = target for building a unit test executable
							for given module of the MCL

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
			cmake --build . --target \<target_name\>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Creating doxygen documentation
The documentation of the MCL is built by doxygen.
The project contains a doxygen configuration file which can be adapted to
your needs. You can build the documentation by calling the cmake target:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cmd}
		cmake --build . --target mcl_doc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Generating distribution package
The distribution package of the MCL is generated using CPack.
CPack is configured in CMake files. You can generate
the distribution package "MCL-<mcl_version>-<target_system>.zip"
by calling the cmake target:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cmd}
		cmake --build . --target package
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Executing unit and integration tests
If you build all targets with including tests you may run tests with

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
		ctest .
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See help of ctest (ctest --help) for running or excluding specific
tests by selecting them with regular expressions.

After the build process you should be able to find all outputs in the
subfolder \<CMAKE_BUILD_DIR\>/build/\<Debug|Release|...\>

### Steps to Build and Run a Custom Agent for Linux
You can apply the following steps to build and run a custom agent using MCL for Linux from scratch.

#### Creating directory structure
Create /usr/customAgentExample directory. This will be the parent directory for the whole process.
Create /usr/customAgentExample/build directory. This will be the parent directory for build outputs.
Create /usr/customAgentExample/build/openssl directory for openssl build outputs.
Create /usr/customAgentExample/build/curl directory for curl build outputs.
Create /usr/customAgentExample/build/mcl directory for mcl build outputs.
Create /usr/customAgentExample/build/agent directory for agent build outputs.
Copy MCL source code .../MCL_Core into /usr/customAgentExample.
Note: Change the modes of directories if needed by
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ sudo chmod 755 <directory_path>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Building OpenSSL
Download OpenSSL package [openssl-1.0.2k.tar.gz](https://www.openssl.org/source/old/1.0.2/)
Unzip openssl-1.0.2k.tar.gz to /usr/customAgentExample.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ cd /usr/customAgentExample/<downloaded_openssl_dir>
	$ ./config --openssldir=/usr/customAgentExample/build/openssl shared -fPIC
	$ sudo make install
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Building libcurl
Download [curl-7.52.1.zip](https://curl.haxx.se/download/)
Unzip curl-7.52.1.zip to /usr/customAgentExample.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ cd /usr/customAgentExample/<downloaded_curl_dir>
	$ LDFLAGS="-Wl,-R/usr/customAgentExample/build/openssl/lib" ./configure --enable-http --with-ssl=/usr/customAgentExample/build/openssl --prefix=/usr/customAgentExample/build/curl --without-libssh2 --disable-ftp --disable-tftp --disable-file --disable-ldap --disable-rtsp --disable-dict --disable-telnet --disable-pop3 --disable-imap --disable-smb --disable-scp --disable-sftp --disable-smtp --disable-gopher --disable-manual
	$ sudo make install
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Building MCL
Create a bash file /usr/customAgentExample/build_mcl with the content:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	#!/bin/bash
	OPENSSL_DIR="/usr/customAgentExample/build/openssl"
	CURL_DIR="/usr/customAgentExample/build/curl"
	MCL_SOURCE_DIR="/usr/customAgentExample/MCL_Core"
	MCL_BUILD_DIR="/usr/customAgentExample/build/mcl"
	if [ -d ${MCL_BUILD_DIR} ]; then
		sudo rm -rf ${MCL_BUILD_DIR}
	fi
	sudo mkdir ${MCL_BUILD_DIR}
	sudo chmod 777 ${MCL_BUILD_DIR}
	cd ${MCL_BUILD_DIR}
	cmake -DCMAKE_PREFIX_PATH="${OPENSSL_DIR};${CURL_DIR}" -DCMAKE_BUILD_TYPE=Release -DMCL_STATICLIB=OFF -DMCL_USE_LIBCURL=ON -DMCL_USE_OPENSSL=ON -DMCL_CREATE_DOXYGEN=OFF -DMCL_TESTING=OFF -DMCL_LOG_UTIL_LEVEL=MCL_LOG_UTIL_LEVEL_NONE ${MCL_SOURCE_DIR}
	cmake --build . --target mc
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Build MCL by
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ sudo ./build_mcl
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Make sure correct libraries are linked with the command:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ ldd /usr/customAgentExample/build/mcl/build/Release/libmc.so
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Building custom agent application
Create /usr/customAgentExample/agent directory for agent application.
Create CMakeLists.txt file in /usr/customAgentExample/agent with the content:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	CMAKE_MINIMUM_REQUIRED(VERSION 3.5 FATAL_ERROR)
	PROJECT(CustomAgentApplication LANGUAGES C)
	SET(CMAKE_C_STANDARD 99)
	SET(CMAKE_C_STANDARD_REQUIRED ON)
	FILE(GLOB SOURCES *.c)
	LIST(APPEND AGENT_SOURCES ${SOURCES})
	SET(MCL "/usr/customAgentExample/build/mcl/build/Release/libmc.so" CACHE INTERNAL "MCL" FORCE)
	SET(MCL_INCLUDE_DIRECTORIES "/usr/customAgentExample/build/mcl/include/" CACHE INTERNAL "MCL_INCLUDE_DIRECTORIES" FORCE)
	SET(AGENT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/build)
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AGENT_OUTPUT_DIR})
	ADD_EXECUTABLE(${PROJECT_NAME} ${AGENT_SOURCES})
	TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${MCL_INCLUDE_DIRECTORIES})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MCL})
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Make sure the provided MCL headers are in /usr/customAgentExample/build/mcl/include/mcl.
Write your custom agent application with source code in /usr/customAgentExample/agent and then run the following commands:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cmd}
	$ cd /usr/customAgentExample/build/agent
	$ cmake /usr/customAgentExample/agent
	$ cmake --build . --clean-first
	$ ./build/CustomAgentApplication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Replacing MCL modules with own implementation
You may want to replace some modules of the MCL project. Such modules
can be listed as following:

		- memory.h
					Implements memory related functions. If not specified
					otherwise alloc, calloc, realloc and free will be used
					from the standard library. You could replace this module
					by your own implementation which handles the memory
					management for your target device and build environment.

		- random.h
					If not specified otherwise MCL will use the random
					generator from the standard library. If you have your
					own implementation for it you may replace this module.

		- security.h
					This module provides several functions which implements
					calculations like SHA256, MD5, RSA key generation etc.
					By replacing this module you can provide your own
					hardware specific implementation. Currently OpenSSL's
					crypto library is used to perform those calculations.

		- http_client.h
					This module is used by the MCL in order to send an HTTP
					request to the MindSphere. In current implementation
					Libcurl is used to realize the communication. If Libcurl
					is not an alternative for your target device you may
					replace this module by your own implementation. Keep
					in mind that your implementation has to fulfill same
					security requirements with respect to TLS v1.2. The
					MindSphere's certificate of course must be also validated
					by your implementation during the SSL handshaking
					procedure.

For replacing a module keep the original *.h file and replace its
implementation file *.c with the same name.

### MCL interfaces and error codes
If you want to use the MCL in your client application (which collects
data and sends them to the MindSphere) then you need to include MCL's
exported interfaces (header files), which are located in
\<MCL_PROJECT_FOLDER\>/include/mcl/\*.h.

In your client application you only need to include "mcl.h" which
internally includes all other required header files.

Most of the MCL functions return an error code which can be used by
the client application to react accordingly. The complete list of error
codes together with their descriptions can be found in mcl_common.h as
well as in doxygen documentation. For error codes returned by a specific
function, please see the function reference in doxygen documentation.

The MCL interfaces are divided basically into following modules according
to their context. The documentation can be referred to get more information about the parameters of each function.

#### Module - Configuration
This module can be used to provide MCL various configuration parameters
which are used in the initialization phase of MCL.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_configuration_initialize(mcl_configuration_t **configuration)
	mcl_configuration_t *mcl_new_configuration()
	E_MCL_ERROR_CODE mcl_configuration_destroy(mcl_configuration_t **configuration)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	mcl_configuration_t is a defined struct with following entries:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}

	typedef struct mcl_configuration_t
	{
		char *mindsphere_hostname;
		mcl_uint16_t mindsphere_port;
		char *mindsphere_certificate;
		char *proxy_hostname;
		mcl_uint16_t proxy_port;
		E_MCL_PROXY proxy_type;
		char *proxy_username;
		char *proxy_password;
		char *proxy_domain;
		E_MCL_SECURITY_PROFILE security_profile;
		mcl_size_t max_http_payload_size;
		char *user_agent;
		char *initial_access_token;
		char *tenant;
		char *store_path;
		mcl_load_registration_information_callback_t load_function;
		mcl_save_registration_information_callback_t save_function;
		mcl_enter_critical_section_callback_t enter_critical_section;
		mcl_leave_critical_section_callback_t leave_critical_section;
	} mcl_configuration_t;

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

__mindsphere_hostname__  
Host name of Mindsphere provided as baseURL in configuration object obtained using MindSphere Launchpad. This parameter is mandatory.

__mindsphere_port__  
Port number of MindSphere which is typically 443. If not set by the user, port will be set to 443 for an https mindsphere_hostname by default.

__mindsphere_certificate__  
Certificate of Mindsphere as a PEM formatted string. This certificate will be used by MCL to verify server identity.
Instead of MindSphere certificate, user can also provide the root certificate or the full certificate chain with this parameter since the server identity will be verified in these two cases as well.
mindsphere_certificate is an optional parameter and it is NULL by default. In this case, MCL will try to use CA certificate store
(provided at build-time of the http client used which is libCurl by default) to verify server identity.

__proxy_hostname__  
Host name of the proxy server if the connection to MindSphere is preferred to be established through a proxy. This is an optional parameter and is NULL by default.
If this parameter is NULL, proxy_port, proxy_type, proxy_username, proxy_password, proxy_domain parameters will be ignored by MCL.

__proxy_port__  
Port number of the proxy server. This is a mandatory parameter if proxy_hostname is provided.

__proxy_type__  
Type of the proxy server. This parameter has effect only if proxy_hostname is provided and has a default value of MCL_PROXY_UNKNOWN.
Check E_MCL_PROXY type defined in mcl_common.h for available options.

__proxy_username__  
User name used in proxy server. This is an optional parameter (NULL by default) and should have a length of maximum 32 characters.

__proxy_password__  
Password for proxy_username. This is a required parameter only if proxy_username is provided and should have a length of maximum 32 characters.

__proxy_domain__  
Domain for proxy_username. This is an optional parameter and domain with length upto 256 characters is accepted.

__security_profile__  
Security profile for the communication with MindSphere. It can take values either MCL_SECURITY_SHARED_SECRET (default) or MCL_SECURITY_RSA_3072.

__max_http_payload_size__  
Maximum payload size in bytes which will be used for all HTTP requests by MCL. With this parameter, user can adopt the size of HTTP requests to use depending on system limitations. The default value is 16384.
Minimum value to set is 400 and maximum value to set is 10485760.

__http_request_timeout__  
Timeout value for HTTP requests in seconds. This is an optional parameter with a default value of 300 seconds.

__user_agent__  
User agent string which will be used in User-Agent header of HTTP requests together with MCL's version string as prefix. It is a mandatory parameter with a length of maximum 256 characters.

__tenant__  
Tenant name on MindSphere. This is a mandatory parameter.

__initial_access_token__  
Initial access token provided in configuration object obtained using MindSphere Launchpad. It is a mandatory parameter for agents which will onboard for the first time.
This parameter will be used by MCL only if registration information is not provided by either load_function or store_path parameters.

__store_path__  
Path of the file to load and save registration information. This parameter is an optional parameter with default value NULL.  
The parameter will be ignored if both save_function and load_function parameters are set.  
If callbacks are set to NULL and store_path parameter is set; file set by store_path parameter will be opened and read to check if registration information exists.
If registration information exists, MCL will be initialized as already onboarded.
If registration information does not exist, valid initial_access_token will be required to onboard and the registration artifacts will be saved to store_path.  
If callbacks are set to NULL, and store_path is also not provided then initial_access_token parameter will be used to onboard but registration artifacts obtained after onboarding will not be saved although the
mcl_communication_t handle will be fully operational until it is destroyed. MCL will not return an error in this case but will log the situation with MCL_LOG_UTIL_LEVEL_INFO log level.  
Keep in mind that using store_path parameter is an unsecure way to save and load registration artifacts and the user is encouraged provide secure callbacks (load_function and save_function) instead.

__load_function__  
User provided function to be used when loading registration information during initialization of MCL.  
This is an optional parameter which is NULL by default. It is used together with save_function parameter. If any one of save_function and load_function is NULL, then both of them will be ignored.  
Check mcl_common.h for function signature.  
The function is expected to set values for every argument provided to it and return MCL_OK in case of success.  
The function is expected to return MCL_REGISTRATION_INFO_IS_NOT_LOADED in case there are no registration information to load and MCL is expected to use initial_access_token parameter to onboard for the first time.  
The function is expected to return MCL_FAIL in case of a failure.

__save_function__  
User provided function to save registration information after onboarding.  
This is an optional parameter which is NULL by default. It is used together with load_function parameter. If any one of save_function and load_function is NULL, then both of them will be ignored.  
Check mcl_common.h for function signature.  
The function is expected to return MCL_OK in case of success.
The function is expected to return MCL_FAIL in case of failure.
If the registration artifacts are not saved, the agent will have to onboard again with a new initial_access_token after the mcl_communication_t handle is destroyed.  

__enter_critical_section__  
User provided function which is called internally by MCL when entering the critical section. The critical section in MCL is the section where the registration information (keys, token, etc.) is accessed.
This is an optional parameter which is NULL by default. User does not have to set it in case concurrent programming is not a requirement.

__leave_critical_section__  
User provided function which is called internally by MCL when leaving the critical section. The critical section in MCL is the section where the registration information (keys, token, etc.) is accessed.
This is an optional parameter which is NULL by default. User does not have to set it in case concurrent programming is not a requirement.

Create an mcl_configuration_t instance using mcl_configuration_initialize function
and then assign each parameter. Pass this mcl_configuration_t instance
to mcl_communication_initialize function to initialize MCL.

You may destroy mcl_configuration_t instance using mcl_configuration_destroy
function once you initialized MCL.

Use of callback functions (load_function and save_function) to save and load
registration artifacts are encouraged instead of store_path for security reasons.

#### Module - Communication
This module can be used to communicate with MindSphere.

You need to call mcl_communication_initialize function passing
configuration parameters (mcl_configuration_t) to it to obtain a communication
handle of type mcl_communication_t. This handle will be used by other functions
in this module.

Every newly created agent on MindSphere needs to be onboarded first using
mcl_communication_onboard function before exchanging any data.

Onboarded agents can rotate their keys using mcl_communication_rotate_key function usually when their keys expire.

Onboarded agents with valid keys can get access token to be used in exchange calls using mcl_communication_get_access_token function.
mcl_communication_get_access_token function is also called within mcl_communication_onboard and mcl_communication_rotate_key functions.

Before you can exchange data with MindSphere it is required to prepare a store
of type mcl_store_t which will contain all data you want to upload. See next section
for details about initializing and adding data to store.

mcl_communication_process is similar to mcl_communication_exchange but additionally, it automatically renews access token if it expires, rotates key if the client secret expires and then retries the exchange operation once more.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_communication_initialize(mcl_configuration_t *configuration, mcl_communication_t **communication)
	E_MCL_ERROR_CODE mcl_communication_onboard(mcl_communication_t *communication)
	E_MCL_ERROR_CODE mcl_communication_rotate_key(mcl_communication_t *communication)
	E_MCL_ERROR_CODE mcl_communication_update_security_information(mcl_communication_t *communication)
	E_MCL_ERROR_CODE mcl_communication_get_access_token(mcl_communication_t *communication)
	E_MCL_ERROR_CODE mcl_communication_exchange(mcl_communication_t *communication, mcl_store_t *store, void **reserved)
	E_MCL_ERROR_CODE mcl_communication_process(mcl_communication_t *communication, mcl_store_t *store, void **reserved)
	E_MCL_ERROR_CODE mcl_communication_destroy(mcl_communication_t **communication)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Store
This module can be used to create a store for all types of data that
your client application wants to exchange with MindSphere. Store is a
container and any type of data is required to be exchanged within this container.
After calling the initialization function a new store will be created which
then can be used to add new items like timeseries, custom data, event or file
to it. When the store is ready to be exchanged you may use the
communication module to start the exchange operation.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_store_initialize(mcl_bool_t streamable, mcl_store_t **store)
	E_MCL_ERROR_CODE mcl_store_destroy(mcl_store_t **store)
	E_MCL_ERROR_CODE mcl_store_new_time_series(mcl_store_t *store, const char *version, const char *configuration_id, const char *routing, mcl_time_series_t **time_series)
	E_MCL_ERROR_CODE mcl_store_new_file(mcl_store_t *store, const char *version, const char *file_path, const char *file_name, const char *file_type, const char *routing, mcl_file_t **file)
	E_MCL_ERROR_CODE mcl_store_new_custom_data(mcl_store_t *store, const char *version, const char *type, const char *routing, mcl_custom_data_t **custom_data)
	E_MCL_ERROR_CODE mcl_store_new_stream_data(mcl_store_t *store, const char *version, const char *type, const char *routing, mcl_stream_data_read_callback_t stream_data_read_callback, void *user_context, mcl_stream_data_t **stream_data)
	E_MCL_ERROR_CODE mcl_store_new_data_source_configuration(mcl_store_t *store, const char *version, mcl_data_source_configuration_t **data_source_configuration)
	E_MCL_ERROR_CODE mcl_store_new_event(mcl_store_t *store, const char *version, const char *type, const char *type_version, E_MCL_EVENT_SEVERITY severity, const char *timestamp, mcl_event_t **event)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Time series
This module can be used to add/modify entries inside a time series which
is created by the store module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_time_series_new_value_set(mcl_time_series_t *time_series, const char *timestamp, mcl_time_series_value_set_t **value_set)
	E_MCL_ERROR_CODE mcl_time_series_add_value(mcl_time_series_value_set_t *value_set, const char *data_point_id, const char *value, const char *quality_code)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Custom data
This module can be used to add/modify entries inside a custom data which
is created by the store module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_custom_data_set_meta_details(mcl_custom_data_t *custom_data, const mcl_json_t *details)
	E_MCL_ERROR_CODE mcl_custom_data_set_payload(mcl_custom_data_t *custom_data, const mcl_uint8_t *content, mcl_size_t content_size)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Stream data
This module can be used to add/modify entries inside a stream data which
is created by the store module. Stream data and custom data are similar,
with only difference that stream data is provided via a callback function.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_stream_data_set_meta_details(mcl_stream_data_t *stream_data, const mcl_json_t *details)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Data source configuration
This module can be used to add/modify entries inside a data source configuration which
is created by the store module.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_source(mcl_data_source_configuration_t *data_source_configuration, const char *name, const char *description, mcl_json_t *custom_data, mcl_data_source_t **data_source)
	E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_point(mcl_data_source_t *data_source, const char *id, const char *name, const char *description, const char *type, const char *unit, mcl_json_t *custom_data)
	E_MCL_ERROR_CODE mcl_data_source_configuration_get_id(mcl_data_source_configuration_t *data_source_configuration, char **id)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Json util
This module can be used to create json strings which may be used by other mcl modules.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_json_util_initialize(E_MCL_JSON_TYPE json_type, const char *string, mcl_json_t **root)
	E_MCL_ERROR_CODE mcl_json_util_start_array(mcl_json_t *root, const char *array_name, mcl_json_t **json_array)
	E_MCL_ERROR_CODE mcl_json_util_get_array_item(mcl_json_t *array, int index, mcl_json_t **item)
	E_MCL_ERROR_CODE mcl_json_util_get_array_size(mcl_json_t *array, int *size)
	E_MCL_ERROR_CODE mcl_json_util_start_object(mcl_json_t *root, const char *object_name, mcl_json_t **json_object)
	E_MCL_ERROR_CODE mcl_json_util_add_string(mcl_json_t *root, const char *object_name, const char *object_value)
	E_MCL_ERROR_CODE mcl_json_util_add_uint(mcl_json_t *root, const char *object_name, const mcl_size_t number)
	E_MCL_ERROR_CODE mcl_json_util_add_float(mcl_json_t *root, const char *object_name, const float number)
	E_MCL_ERROR_CODE mcl_json_util_add_double(mcl_json_t *root, const char *object_name, const double number)
	E_MCL_ERROR_CODE mcl_json_util_add_bool(mcl_json_t *root, const char *object_name, const mcl_bool_t bool_value)
	E_MCL_ERROR_CODE mcl_json_util_add_null(mcl_json_t *root, const char *object_name)
	E_MCL_ERROR_CODE mcl_json_util_add_object(mcl_json_t *root, const char *object_name, mcl_json_t *object)
	E_MCL_ERROR_CODE mcl_json_util_add_item_to_array(mcl_json_t *root, mcl_json_t *object)
	E_MCL_ERROR_CODE mcl_json_util_get_object_item(mcl_json_t *json_parent, const char *child_name, mcl_json_t **json_child)
	E_MCL_ERROR_CODE mcl_json_util_has_child(mcl_json_t *root, mcl_bool_t *result)
	E_MCL_ERROR_CODE mcl_json_util_get_number_value(mcl_json_t *json, mcl_int32_t *number_value)
	E_MCL_ERROR_CODE mcl_json_util_get_double_value(mcl_json_t *json, double *double_value)
	E_MCL_ERROR_CODE mcl_json_util_get_string(mcl_json_t *json_item, char **string_value)
	E_MCL_ERROR_CODE mcl_json_util_to_string(mcl_json_t *root, char **json_string)
	E_MCL_ERROR_CODE mcl_json_util_parse(const char *json_string, mcl_json_t **root)
	void mcl_json_util_finish_array(mcl_json_t **json_array)
	void mcl_json_util_finish_object(mcl_json_t **json_object)
	void mcl_json_util_destroy(mcl_json_t **root)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Random
This module provides a single function to generate globally unique identifier.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_random_generate_guid(char **guid)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Logging
MCL enables you to configure how log messages will be stored according
to your needs. It is possible to decide during compilation of MCL whether
log messages for a certain log level will be removed. Any log message
statements in MCL with lower log level than the selected one will be
replaced by no-op instruction. Thus, the size of the MCL can be reduced.
In order to set the log level for compilation please refer to the option
MCL_LOG_UTIL_LEVEL in above cmake command options.

The log level in compilation time can be used together with
the runtime log level in order to achive desired logging
behaviour. Log level in compilation time shadows the runtime log level, e.g.
if you set during compilation time the log level to ERROR you won't be able
to log DEBUG messages at runtime. In this example you can only decide whether
ERROR, FATAL or NONE log messages are active during runtime.

For adjusting the logging mechanism use the following functions:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
	E_MCL_ERROR_CODE mcl_log_util_set_output_level(const int log_level)
	E_MCL_ERROR_CODE mcl_log_util_initialize(E_LOG_OUTPUT log_output, ...)
	const char *error_string mcl_log_util_convert_error_code_to_string(E_MCL_ERROR_CODE error_code)
	void mcl_log_util_finalize(void)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#### Module - Event
This module can be used to set optional parameters of an event that will be uploaded to MindSphere.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
E_MCL_ERROR_CODE mcl_event_set_option(mcl_event_t *event, E_MCL_EVENT_OPTION option, const void *value)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Examples
A typical agent use case would require these steps:

__Initialization:__  
1\.   Create a new configuration handle (mcl_configuration_t) using mcl_configuration_initialize function.  
2\.   Assign values for each parameter in mcl_configuration_t handle. You can omit optional parameters.  
3\.   Initialize MCL with the mcl_configuration_t handle using mcl_communication_initialize function
	which will create a new mcl_communication_t handle.  
	Note that mcl_configuration_t handle will not be necessary after initialization of MCL,
	so you can destroy it using mcl_configuration_destroy function.  
__Onboarding:__  
4\.   Pass the mcl_communication_t handle to mcl_communication_onboard function to onboard the agent.  
	Continue with the following steps only if this function returns MCL_OK or MCL_ALREADY_ONBOARDED.  
Store Initialization:  
5\.   Initialize an mcl_store_t handle using mcl_store_initialize function.  
	This will be the container for your exchange data.  
__Data Source Configuration Upload:__  
6\.   Create a new data source configuration in the store using mcl_store_new_data_source_configuration function
	which will return mcl_data_source_configuration_t handle.  
7\.   Use mcl_data_source_configuration_t handle to add data source to data source configuration with mcl_data_source_configuration_add_data_source function.  
	This function will return mcl_data_source_t handle.  
8\.   Use mcl_data_source_t handle to add data point(s) to data source with mcl_data_source_configuration_add_data_point function.  
	This function will ask for a globally unique identifier for each data point added which can be generated using mcl_random_generate_guid function.  
	User must keep data source configuration ID (randomly generated by MCL internally and can be accessed with mcl_data_source_configuration_get_id function)
	and data point ID(s) for timeseries upload.  
9\.   Call mcl_communication_exchange function to upload the store containing the data source configuration.  
	Make sure data source configuration is uploaded before the first timeseries upload.  
	Data source configuration is uploaded only once for an agent as long as the data source configuration is not changed.  
	If the exchange is successful (i.e. the function returns MCL_OK), data source configuration will be automatically deleted from the store.  
	You are expected to destroy the store using mcl_store_destroy function if you are not going to use it for further exchange operations.  
	After data source configuration upload, you will need to do data point mapping from MindSphere Launchpad before starting to upload timeseries data.  
__Timeseries and File Upload:__  
10\.  Create timeseries in the store (you can use the one initialized in Step-5 or create a new one) using mcl_store_new_time_series function.
	You will need data source configuration ID, see Step-8.  
11\.  Add a new value set to timeseries using mcl_time_series_new_value_set function for every timestamp you want exchange data for.  
12\.  Set values for every data point in each value set using mcl_time_series_add_value.  
	Remember that data points in each value set correspond to the data points previously added to data source configuration.  
	You can exchange this data just like you did in Step-9 or you can follow the next steps before an exchange to upload timeseries and file data together.  
13\.  Create file in the store using mcl_store_new_file function.  
	Note that file_path parameter of mcl_store_new_file function is the full path of the file in your system you want to upload
	and file_name parameter is the name of the file to be used on MindSphere.  
	MCL opens and reads data from file_path and uploads its content to MindSphere as a file with name file_name.  
14\.  Call mcl_communication_exchange function to upload the store containing both timeseries and file data.  
	Note that if your token to exchange data is expired, you get MCL_UNAUTHORIZED error from the mcl_communication_exchange function.  
	In that case, you can try to get new access token using mcl_communication_get_access_token function.  
15\.  Destroy the store using mcl_store_destroy function.  
16\.  Destroy mcl_communication_t handle using mcl_communication_destroy function.  

You can find custom agent examples using MCL in /examples folder of MCL distribution.

#### Example : Onboard and Data Source Configuration Upload
This example onboards a newly created agent, checks if the onboarding is successful, then creates and uploads a data source configuration.
Check /examples/onboard_dsc_upload.c file for the implementation.

#### Example : Timeseries Upload
This example initializes MCL for an already onboarded agent of which data source configuration is also uploaded and then uploads timeseries data.
Check /examples/timeseries_upload.c file for the implementation.

#### Example : File Upload
This example initializes MCL for an already onboarded agent and then uploads a file.
Check /examples/file_upload.c file for the implementation.

#### Example : Event
This example onboards a newly created agent, then uploads events to MindSphere.
Check /examples/event.c file for the implementation.

#### Example : Multiprocess Agent
This example agent initializes MCL in multiple processes with the same registration information and exchanges data with MindSphere.
Check /examples/multi_process_agent.c,  /examples/critical_section.cpp and /examples/critical_section.h file for the implementation.
Note that this example is a Windows application.

### Licensing
Please see the files called LICENSE.md and ReadMe_OSS.htm.
