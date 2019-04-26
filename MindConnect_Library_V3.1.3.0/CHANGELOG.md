# [3.1.3.0] (2019-01-11)

### Bug Fixes

* Custom data version format limitation is removed.

### Security

* Minimum required OpenSSL version is updated from 1.0.2k to 1.0.2q for users using OpenSSL as the crypto library.

# [3.1.2.0] (2018-10-11)

### Features

* Events with version 2.0 can be exchanged with MindSphere.

# [3.1.1.0] (2018-09-10)

### Bug Fixes

* Redundant character in resulting string in string concatenation is removed.

# [3.1.0.0] (2018-06-07)

### Features

* Event type is introduced which can be exchanged with MindSphere.

### Bug Fixes

* "version" parameter's format was not checked
* Doxygen comment about "unit" parameter was wrong
* "configuration_id" parameter was optional

# [3.0.1.0] (2018-04-11)

### Features

* mcl_communication_process() implemented which performs a series of steps, such as get access token and key rotation if exchange fails.

### Refactor

* Variable declarations moved to beginning of their scopes to be compatible with C98.

# [3.0.0.0] (2018-02-22)

### Features

* First release on AWS.

