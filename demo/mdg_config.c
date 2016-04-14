/*
 * Include this in your chat demo main, set desired config...
 */


/******************************************************************************
 *** Example configuration for test server ************************************
 ******************************************************************************/

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


/******************************************************************************
 *** Configuration for localhost **********************************************
 ******************************************************************************/

static const mdg_control_server_endpoint localhost = { "127.0.0.1", 1443 };
static const mdg_control_server_endpoint *mdg_control_hosts_localhost[] = {
  &localhost,
  0
};

// Localhost server public key.
static const uint8_t localhost_ctrl_server_key[32] = {
  42,98,160,215,150,65,32,170,90,23,191,176,134,18,218,
  90,245,223,150,146,11,208,203,39,15,142,206,117,187,
  126,191,123
};
// Example config for trust server. Lists only localhost server key.
static const uint8_t *trusted_server_keys_localhost[] = {
  localhost_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_localhost = {
  mdg_control_hosts_localhost,
  trusted_server_keys_localhost,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};


/******************************************************************************
 **** Configuration for tmdg82 ************************************************
 ******************************************************************************/

static const mdg_control_server_endpoint tmdg82 = { "172.30.252.72", 443 };
static const mdg_control_server_endpoint *mdg_control_hosts_tmdg82[] = {
  &tmdg82,
  0
};

// Tmdg82 server public key.
static const uint8_t tmdg82_ctrl_server_key[32] = {
  42,98,160,215,150,65,32,170,90,23,191,176,134,18,218,
  90,245,223,150,146,11,208,203,39,15,142,206,117,187,
  126,191,123
};
// Example config for trust server. Lists only tmdg82 server key.
static const uint8_t *trusted_server_keys_tmdg82[] = {
  tmdg82_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_tmdg82 = {
  mdg_control_hosts_tmdg82,
  trusted_server_keys_tmdg82,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};


/******************************************************************************
 *** Config for "prod 01" (SDG) environment ***********************************
 ******************************************************************************/

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
static const uint8_t prod01_ctrl_server_key[32] = {
  126,178,25,148,11,236,217,80,
  255,253,222,17,71,41,169,2,
  206,237,147,207,197,225,18,181,
  213,45,170,24,231,165,61,48
};

// Trust server key:
static const uint8_t *trusted_server_keys_prod01[] = {
  prod01_ctrl_server_key,
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




/******************************************************************************
 *** Config for Danfoss (MDG) environment *************************************
 ******************************************************************************/

static const mdg_control_server_endpoint dkhc01mdg01 = { "77.66.11.90", 443 };
static const mdg_control_server_endpoint dkhc01mdg02 = { "77.66.11.92", 443 };
static const mdg_control_server_endpoint dkhc02mdg01 = { "5.179.92.180", 443 };
static const mdg_control_server_endpoint dkhc02mdg02 = { "5.179.92.182", 443 };
static const mdg_control_server_endpoint *mdg_control_hosts_danfoss_prod[] = {
  &dkhc01mdg01,
  &dkhc01mdg02,
  &dkhc02mdg01,
  &dkhc02mdg02,
  0
};

// Server public key.
static const uint8_t danfoss_prod_ctrl_server_key[32] = {
  126,178,25,148,11,236,217,80,
  255,253,222,17,71,41,169,2,
  206,237,147,207,197,225,18,181,
  213,45,170,24,231,165,61,48
};

// Trust server key:
static const uint8_t *trusted_server_keys_danfoss_prod[] = {
  danfoss_prod_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_danfoss_prod = {
  mdg_control_hosts_danfoss_prod,
  trusted_server_keys_danfoss_prod,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};


/******************************************************************************
 *** Config for Danfoss-QA machine ********************************************
 ******************************************************************************/

static const mdg_control_server_endpoint dkhc02mdg03 = { "5.179.92.184", 443 };
static const mdg_control_server_endpoint *mdg_control_hosts_danfoss_qa[] = {
  &dkhc02mdg03,
  0
};

// Server public key.
static const uint8_t danfoss_qa_ctrl_server_key[32] = {
  126,178,25,148,11,236,217,80,
  255,253,222,17,71,41,169,2,
  206,237,147,207,197,225,18,181,
  213,45,170,24,231,165,61,48
};

// Trust server key:
static const uint8_t *trusted_server_keys_danfoss_qa[] = {
  danfoss_qa_ctrl_server_key,
  0
};

const mdg_configuration mdg_configuration_danfoss_qa = {
  mdg_control_hosts_danfoss_qa,
  trusted_server_keys_danfoss_qa,
#if defined(MDG_USE_MDNS) && !defined(__MACH__)
  // Enable mDNS, except from OSX/ios for now.
  "unix-chat-mgd"
#else
  0 //Use no mDNS name.
#endif
};
