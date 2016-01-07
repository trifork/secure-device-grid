/*
 * Include this in your chat demo main, set desired config...
 */

/*
 * Example configuration for test server:
 */
static const mdg_control_server_endpoint dkhc01mdg03 = { "77.66.11.95", 443 };
static const mdg_control_server_endpoint *mdg_control_hosts_test[] = {
  &dkhc01mdg03,
  0
};

// Test server public key.
static const uint8_t test_ctrl_server_key[32] = {
  81,13,101,52,29,109,136,196,
  86,91,34,91,3,19,150,3,
  215,43,210,9,242,146,119,188,
  153,245,78,232,94,113,37,47
};
// Example config for trust server. Lists only test server key.
static const uint8_t *trusted_server_keys_test[] = {
  test_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_test = {
  mdg_control_hosts_test,
  trusted_server_keys_test,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};


/*
 * Config for "prod 01" environment.
 */
static const mdg_control_server_endpoint dkhc01sdg01 = { "77.66.7.91", 443 };
static const mdg_control_server_endpoint dkhc01sdg02 = { "77.66.7.92", 443 };
static const mdg_control_server_endpoint dkhc02sdg01 = { "5.179.92.177", 443 };
static const mdg_control_server_endpoint dkhc02sdg02 = { "5.179.92.178", 443 };
static const mdg_control_server_endpoint *mdg_control_hosts_prod01[] = {
  &dkhc01sdg01,
  &dkhc01sdg02,
  &dkhc02sdg01,
  &dkhc02sdg02,
  0
};
// Server public key.
static const uint8_t prod_ctrl_server_key[32] = {
  81,13,101,52,29,109,136,196,
  86,91,34,91,3,19,150,3,
  215,43,210,9,242,146,119,188,
  153,245,78,232,94,113,37,47
};
// Trust server key:
static const uint8_t *trusted_server_keys_prod01[] = {
  prod_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_prod01 = {
  mdg_control_hosts_prod01,
  trusted_server_keys_prod01,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};

