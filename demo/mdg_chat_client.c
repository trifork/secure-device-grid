#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
// For sleep:
#if __linux
#include <unistd.h>
#elif MDG_WINDOWS
#include <Windows.h>
#endif

#include "mdg_peer_api.h"
#include "mdg_peer_storage.h"

extern void hex_encode_bytes(const uint8_t *input, char *dst, int input_size);
extern int hex_decode_bytes(char *input, uint8_t *dst, int output_size);
extern void mdg_chat_output_fprintf(const char *fmt, ...);
extern void mdg_chat_client_exit();

typedef struct {
  uint8_t peer_id[MDG_PEER_ID_SIZE];
} pairing_t;

#define MAX_CHAT_PAIRINGS 32
static pairing_t chat_pairings[MAX_CHAT_PAIRINGS];
static int chat_pairings_count = 0;

static int pcr_timeout = 10;
static int incomingcall_testdelay = 0;

struct cmd {
  char *short_name;
  char *long_name;
  char *help_text;
  void (*handler)(char *args_buf, int len);
};

static void save_all_pairings_to_file()
{
  FILE *f = fopen("chat_demo_pairings.bin", "wb");
  if (f != NULL) {
    fwrite(&chat_pairings_count, 1, sizeof(chat_pairings_count), f);
    fwrite(chat_pairings, 1, sizeof(chat_pairings), f);
    fclose(f);
  }
}

static void load_all_pairings_from_file()
{
  static int has_loaded = 0;
  if (!has_loaded) {
    has_loaded = 1;
    FILE *f = fopen("chat_demo_pairings.bin", "rb");
    if (f != NULL) {
      int s = fread(&chat_pairings_count, 1, sizeof(chat_pairings_count), f);
      if (s == sizeof(chat_pairings_count)) {
        s = fread(chat_pairings, 1, sizeof(chat_pairings), f);
        if (s != sizeof(chat_pairings)) {
          // Make warning on s get ignored.
          has_loaded = 2;
        }
      }
      fclose(f);
    }
  }
}

char client_email[256];
extern char mdg_chat_platform[];

mdg_property_t chatclient_client_props[] = {
  // Example properties. Make up your own for your application.
  {"client_app_prop1", "mdg_chat_client" },
  {"platform", mdg_chat_platform },
  {"client_email", client_email },
  {0, 0 },
};

static int arg_decode_gotonext(char **args_buf, int *lenp)
{
  int i, len = *lenp;
  char *p = *args_buf;
  if (len <= 0) {
    return 1;
  }
  for (i = 0; i < len; i++) {
    if (p[i] == ' ') {
      break;
    }
  }
  p[i] = 0;
  if (i < len) {
    i++;
  }
  for ( ; i < len; i++) {
    if (p[i] != ' ') {
      break;
    }
  }
  *args_buf = &p[i];
  *lenp -= i;
  return 0;
}

static int arg_decode_device_id_hex(char **args_buf, int *len, uint8_t *target)
{
  char *dev_id_arg = *args_buf;
  char *first_space = strstr(dev_id_arg, " ");
  int arg_len;
  if (*len < MDG_PEER_ID_SIZE * 2) {
    return 2;
  }
  if (arg_decode_gotonext(args_buf, len)) {
    return 1;
  }
  if (first_space != 0) {
    arg_len = first_space - dev_id_arg;
  } else {
    arg_len = *args_buf - dev_id_arg;
  }
  if (arg_len != MDG_PEER_ID_SIZE * 2) {
    return 3;
  }
  if (hex_decode_bytes(dev_id_arg, target, MDG_PEER_ID_SIZE)) {
    return 4;
  }

  return 0;
}

static void quit_handler(char *args_buf, int len)
{
  mdg_chat_output_fprintf("got quit command, exit(0)\n");
  mdg_chat_client_exit();
}

static void whoami_handler(char *args_buf, int len)
{
  mdg_peer_id_t device_id;
  char hexed[128];
  if (mdg_whoami(&device_id) != 0) {
    mdg_chat_output_fprintf("whoami failed.\n");
    return;
  }

  hex_encode_bytes(device_id, hexed, MDG_PRIVATE_KEY_DATA_SIZE);
  mdg_chat_output_fprintf("whoami says %s\n", hexed);
}



static void status_handler(char *args_buf, int len)
{
  int s;
  mdg_status_t status;
  s = mdg_status(&status);
  if (s != 0) {
    mdg_chat_output_fprintf("status failed with return %d\n", s);
    return;
  }
  mdg_chat_output_fprintf("=status:\n");
  mdg_chat_output_fprintf("unixtime_utc:     %d\n", (int) status.unixtime_utc);
  mdg_chat_output_fprintf("connect_switch:   %d\n", (int)status.connect_switch);
  mdg_chat_output_fprintf("connected_to_mdg: %d\n", (int)status.connected_to_mdg);
  mdg_chat_output_fprintf("pairing_mode:     %d\n", (int)status.pairing_mode);
  mdg_chat_output_fprintf("aggressive_ping:  %d\n", (int)status.aggressive_ping);
  mdg_chat_output_fprintf("remote_logging:   %d\n", (int)status.remote_logging_enabled);
}

static void connect_handler(char *args_buf, int len)
{
  if (mdg_connect(chatclient_client_props) != 0) {
    mdg_chat_output_fprintf("mdg_chat_client: connect failed\n");
  }
}

void mdg_chat_init()
{
  connect_handler(0, 0);
}

static void disconnect_handler(char *args_buf, int len)
{
  if (mdg_disconnect() != 0) {
    mdg_chat_output_fprintf("mdg_chat_client: disconnect failed\n");
  }
}

static int arg_parse_device_public_key(char **args_buf, int *len, uint8_t *device_id)
{
  if (*len < 1) {
    mdg_chat_output_fprintf("could not parse arg as public key\n");
    return 1;
  }
  if (arg_decode_device_id_hex(args_buf, len, device_id)) {
    mdg_chat_output_fprintf("could not parse arg as public key\n");
    return 1;
  }
#if DEBUG
  char hexed[128];
  hex_encode_bytes(device_id, hexed, MDG_PRIVATE_KEY_DATA_SIZE);
  mdg_chat_output_fprintf("parsed arg:device with public id %s\n", hexed);
#endif
  return 0;
}

static void pair_remote_handler(char *args_buf, int len)
{
  char *otp_arg = args_buf;
  int s;
  if (arg_decode_gotonext(&args_buf, &len)) {
    mdg_chat_output_fprintf("pair remote: Missing required arg, OTP.\n");
    return;
  }
  s = mdg_pair_remote(otp_arg);
  if (s) {
    mdg_chat_output_fprintf("mdg_pair_remote failed with %d\n", s);
  }
}

static void aggressive_ping_handler(char *args_buf, int len)
{
  char *duration_arg = args_buf;
  long duration;
  int s;
  if (!arg_decode_gotonext(&args_buf, &len)) {
    duration = strtol(duration_arg, 0, 10);
  } else {
    duration = 120;
  }

  s = mdg_aggressive_ping(duration);

  if (s != 0) {
    mdg_chat_output_fprintf("mdg_aggressive_ping failed, s=%d\n", s);
  } else if (duration == 0) {
    mdg_chat_output_fprintf("mdg_aggressive_ping stopped.\n");
  } else {
    mdg_chat_output_fprintf("mdg_aggressive_ping for %ld seconds started\n", duration);
  }
}

static char preset_otp_buffer[MDG_PRESET_PAIR_ID_SIZE];
static void open_for_pairing_handler(char *args_buf, int len)
{
  char *duration_arg = args_buf;
  long duration;
  int s;
  if (!arg_decode_gotonext(&args_buf, &len)) {
    duration = strtol(duration_arg, 0, 10);
  } else {
    duration = 120;
  }

  preset_otp_buffer[0] = 0; // Do not use preset otp.
  mdg_disable_pairing_mode();
  s = mdg_enable_pairing_mode(duration);

  if (s != 0) {
    mdg_chat_output_fprintf("Open for pairing failed, s=%d\n", s);
  } else {
    mdg_chat_output_fprintf("Open for pairing for %ld seconds\n", duration);
  }
}

static void open_for_pairing_handler_preset_otp(char *args_buf, int len)
{
  char *otp_arg = args_buf;
  long duration = 120;
  int s;
  if (!arg_decode_gotonext(&args_buf, &len)) {
    int otp_len = strlen(otp_arg);
    if (otp_len < MDG_PRESET_PAIR_ID_SIZE) {
      memcpy(preset_otp_buffer, otp_arg, otp_len);
      preset_otp_buffer[otp_len] = 0;
    } else {
      mdg_chat_output_fprintf("OTP argument too long. Limit=%d\n", MDG_PRESET_PAIR_ID_SIZE);
      return;
    }
  } else {
    mdg_chat_output_fprintf("OTP argument required.\n");
    return;
  }

  mdg_disable_pairing_mode();
  s = mdg_enable_pairing_mode(duration);

  if (s != 0) {
    mdg_chat_output_fprintf("Open for pairing failed, s=%d\n", s);
  } else {
    mdg_chat_output_fprintf("Open for pairing for %ld seconds\n", duration);
  }
}


static int set_intparam_handler(char **args_buf, int *len,
                                 char *param_name,
                                 int *target, int min, int max)
{
  long value;
  char *value_arg = *args_buf;

  if (arg_decode_gotonext(args_buf, len)) {
    mdg_chat_output_fprintf("Missing required arg, %s.\n", param_name);
    return 1;
 }
  value = strtol(value_arg, 0, 10);
  if (min <= value && value <= max) {
    *target = (int)value;
    return 0;
  } else {
    mdg_chat_output_fprintf("Illegal %s value.\n", param_name);
    return 1;
  }
}

static void set_pc_timeout_handler(char *args_buf, int len)
{
  if (!set_intparam_handler(&args_buf, &len, "timeout",
                            &pcr_timeout, 1, 600)) {
    mdg_chat_output_fprintf("Set place_call timeout to %d\n",
            pcr_timeout);
  }
}

static void test_random_handler(char *args_buf, int len)
{
  uint8_t buffer[MDG_PEER_ID_SIZE];
  char hexed[2*MDG_PEER_ID_SIZE + 1];

  mdg_make_private_key(buffer);
  hex_encode_bytes(buffer, hexed, sizeof(buffer));
  mdg_chat_output_fprintf("%s\n", hexed);
}

static void set_incomingcall_testdelay_handler(char *args_buf, int len)
{
  if (!set_intparam_handler(&args_buf, &len, "incomingcall_testdelay",
                            &incomingcall_testdelay, 0, 6000)) {
    mdg_chat_output_fprintf("Set incomingcall_testdelay to %d\n",
            incomingcall_testdelay);
  }
}

static void close_conn_handler(char *args_buf, int len)
{
  int conn_id, s;
  if (!set_intparam_handler(&args_buf, &len, "connection_id",
                            &conn_id,
                            -1, 1000)) {
    mdg_chat_output_fprintf("Invoking mdg_close_peer_connection(%d)\n", conn_id);
    if ((s = mdg_close_peer_connection(conn_id))) {
      mdg_chat_output_fprintf("mdg_close_peer_connection returned %d\n", s);
    }
  }
}


static void conninfo_handler(char *args_buf, int len)
{
  int conn_id, s;
  uint8_t sender_device_id[MDG_PEER_ID_SIZE];

  if (!set_intparam_handler(&args_buf, &len, "connection_id",
                            &conn_id,
                            -1, 1000)) {
    s = mdg_get_connection_info(conn_id, &sender_device_id);
    if (s != 0) {
      mdg_chat_output_fprintf("mdg_get_connection_info failed, returned %d\n", s);
    } else {
      char hexed[2* MDG_PEER_ID_SIZE + 1];
      hex_encode_bytes(sender_device_id, hexed, MDG_PEER_ID_SIZE);
      mdg_chat_output_fprintf("Peer for connection is %s\n", hexed);
    }
  }
}

extern void save_demo_email();

static void email_handler(char *args_buf, int len)
{
  char *a1 = args_buf;
  a1[len] = 0;
  if (arg_decode_gotonext(&args_buf, &len)) {
    mdg_chat_output_fprintf("Email for next connect is %s\n", client_email);
    return;
  }
  strncpy(client_email, a1, sizeof(client_email));
  client_email[sizeof(client_email) - 1] = 0;
  mdg_chat_output_fprintf("Email for next connect set to %s\n", client_email);
  disconnect_handler(0, 0);
  connect_handler(0, 0);
  save_demo_email();
}

static void send_handler(char *args_buf, int len)
{
  int conn_id, s;
  char *message;

  if (!set_intparam_handler(&args_buf, &len, "connection_id",
                            &conn_id,
                            -1, MDG_MAX_CONNECTION_COUNT + 1)) {
    message = args_buf;
    s = mdg_send_to_peer((uint8_t*) message, len, conn_id);
    if (s) {
      mdg_chat_output_fprintf("send-data-to-peer failed with %d\n", s);
    }
  }
}

static void send_hex_handler(char *args_buf, int len)
{
  int conn_id, s;
  uint8_t data[2048];

  if (!set_intparam_handler(&args_buf, &len, "connection_id",
                            &conn_id,
                            -1, MDG_MAX_CONNECTION_COUNT + 1)) {
    if (len < 2
        || (len & 1) != 0
        || (len / 2) > sizeof(data)
        || hex_decode_bytes(args_buf, data, len / 2) != 0) {
      mdg_chat_output_fprintf("data argument must be hex encoded, even number of chars, max %d bytes.\n",
                  (int)sizeof(data));
      return;
    }
    s = mdg_send_to_peer(data, len / 2, conn_id);
    if (s) {
      mdg_chat_output_fprintf("mdg_send_data_to_peer failed with %d\n", s);
    }
  }
}

static void pretty_print_otp(const char *format, const char *otp)
{
  char c, pretty[32], *p=pretty;
  int i = 0;
  while (1) {
    c = otp[i++];
    if (c == 0) {
      *p = 0;
      break;
    }
    if (i > 1 && (i % 3) == 1) {
      *p++ = '-';
    }
    *p++ = c;
  }
  mdg_chat_output_fprintf(format, pretty);
}

// Callback invoked by MDG:
void mdguser_control_status(mdg_control_status_t state)
{
  switch (state) {
  case mdg_control_not_desired:
    mdg_chat_output_fprintf("Callback: mdguser_control_status state=not_desired\n");
    break;
  case mdg_control_connecting:
    mdg_chat_output_fprintf("Callback: mdguser_control_status state=connecting\n");
    break;
  case mdg_control_connected:
    mdg_chat_output_fprintf("Callback: mdguser_control_status state=connected\n");
    break;
  case mdg_control_failed:
    mdg_chat_output_fprintf("Callback: mdguser_control_status state=failed\n");
    break;
  default:
    mdg_chat_output_fprintf("Callback: mdguser_control_status invoked with unknown state=%d\n", state);
  }
}

// Callback invoked by MDG:
void mdguser_pairing_state(mdg_pairing_mode_state_t* state, mdg_pairing_status status)
{
  switch (status) {
  case mdg_pairing_starting:
    mdg_chat_output_fprintf("Callback: mdguser_pairing_state status=mdg_pairing_starting\n");
    break;
  case mdg_pairing_otp_ready:
    pretty_print_otp("Chat client: One-Time-Password (OTP) for pairing is: %s\n", state->otp);
    break;
  case mdg_pairing_completed:
    {
      char hexed[2 * MDG_PEER_ID_SIZE + 1];
      hex_encode_bytes(state->new_peer, hexed, MDG_PEER_ID_SIZE);
      mdg_chat_output_fprintf("Callback: mdguser_pairing_state status=mdg_pairing_completed. Peer: \"%s\"\n", hexed);
    }
    break;
  case mdg_pairing_failed_generic:
    mdg_chat_output_fprintf("Callback: mdguser_pairing_state status=mdg_pairing_failed_generic\n");
    break;
  case mdg_pairing_failed_otp:
    mdg_chat_output_fprintf("Callback: mdguser_pairing_state status=mdg_pairing_failed_otp\n");
    break;
  default:
    mdg_chat_output_fprintf("Callback: mdguser_pairing_state invoked with unknown status=%d\n", status);
    break;
  }
}

static void cancel_pairing_mode_handler(char *args_buf, int len)
{
  mdg_disable_pairing_mode();
}

static void list_pairings_handler(char *args_buf, int len)
{
  int i;
  char hexed[129];
  load_all_pairings_from_file();
  mdg_chat_output_fprintf("Listing pairings. Count=%d\n", chat_pairings_count);
  for (i = 0; i < chat_pairings_count; i++) {
    hex_encode_bytes(chat_pairings[i].peer_id, hexed, MDG_PEER_ID_SIZE);
    mdg_chat_output_fprintf("%s\n", hexed);
  }
}

static void remove_pairing_handler(char *args_buf, int len)
{
  unsigned char peer_id[MDG_PEER_ID_SIZE];
  if (!arg_parse_device_public_key(&args_buf, &len, peer_id)) {
    int s = mdg_revoke_pairing(peer_id);
    mdg_chat_output_fprintf("mdg_revoke_pairing returned %d\n", s);
  }
}

static void add_test_pairing_handler(char *args_buf, int len)
{
  uint8_t peer_id[MDG_PEER_ID_SIZE];
  if (!arg_parse_device_public_key(&args_buf, &len, peer_id)) {
    int s = mdgstorage_add_pairing(peer_id);
    mdg_chat_output_fprintf("mdgstorage_add_pairing returned %d\n", s);
  }
}

static void place_call_remote_handler(char *args_buf, int len)
{
  uint8_t device_id[MDG_PEER_ID_SIZE];
  char *protocol_arg;
  int s;
  uint32_t connection_id;

  arg_parse_device_public_key(&args_buf, &len, device_id);

  if (len > 0) {
      protocol_arg = args_buf;
      if (arg_decode_gotonext(&args_buf, &len)) {
        mdg_chat_output_fprintf("could not parse optional argument protocol\n");
        return;
      }
  } else {
    protocol_arg = "chat-client";
  }

  s = mdg_place_call_remote(device_id, protocol_arg, &connection_id, pcr_timeout);
  if (s == 0) {
    mdg_chat_output_fprintf("Place call started, got connection_id=%d\n", connection_id);
  } else {
    mdg_chat_output_fprintf("Place call failed, got error=%d\n", s);
  }
}

static void pair_local_handler(char *args_buf, int len)
{
	int s;
	char *otp_arg;
	char* addresses[2] = {0, 0};
	int port;
	mdg_ip_addr_t peer_ip;
	peer_ip.h_addr_list = addresses;
	
	otp_arg = args_buf;
	if (arg_decode_gotonext(&args_buf, &len)) {
		mdg_chat_output_fprintf("pair local: Missing required arg, OTP.\n");
		return;
	}

	addresses[0] = args_buf;
	if (arg_decode_gotonext(&args_buf, &len)) {
		mdg_chat_output_fprintf("pair local: Missing required arg, host.\n");
		return;
	}

	if (set_intparam_handler(&args_buf, &len, "port", &port, 1, 0xffff) != 0) {
		return;
	}

	s = mdg_pair_local(otp_arg, peer_ip, (uint16_t) port);
	if (s != 0) {
		mdg_chat_output_fprintf("mdg_pair_local failed with %d\n", s);
	}
}

static void place_call_local_handler(char *args_buf, int len)
{
	int s;
	char* addresses[2] = { 0, 0 };
	char *protocol_arg;
	uint8_t device_id[MDG_PEER_ID_SIZE];
	uint32_t connection_id;
	int port;
	mdg_ip_addr_t peer_ip;
	peer_ip.h_addr_list = addresses;

	s = arg_parse_device_public_key(&args_buf, &len, device_id);
	if (s != 0) {
		return;
	}

	addresses[0] = args_buf;
	if (arg_decode_gotonext(&args_buf, &len)) {
		mdg_chat_output_fprintf("place call local: Missing required arg, host.\n");
		return;
	}

	if (set_intparam_handler(&args_buf, &len, "port", &port, 1, 0xffff) != 0) {
		return;
	}

	if (len > 0) {
		protocol_arg = args_buf;
		if (arg_decode_gotonext(&args_buf, &len)) {
			mdg_chat_output_fprintf("place call local: Could not parse optional arg, protocol.\n");
			return;
		}
	} else {
		protocol_arg = "chat-client";
	}

	s = mdg_place_call_local(device_id, protocol_arg, peer_ip, (uint16_t) port, &connection_id, pcr_timeout);
	if (s == 0) {
		mdg_chat_output_fprintf("Place call started, got connection_id=%d\n", connection_id);
	} else {
		mdg_chat_output_fprintf("Place call failed, got error=%d\n", s);
	}
}

static void basic_help_handler(char *args_buf, int len);
static void advanced_help_handler(char *args_buf, int len);
static struct cmd advanced_commands[] = {
  {"/c", "/connect",
   "Set connection-wanted flag to true",
   connect_handler },
  {"/d", "/disconnect",
   "set connection-wanted flag to false",
   disconnect_handler },
  {"/atp", "/add-test-pairing",
   "Add (fake) pairing, provide device-id as arg",
   add_test_pairing_handler },
  {"/agp", "/agressive_ping",
   "Invokes mdg_aggressive_ping, provide duration as optional arg, 0 to cancel",
   aggressive_ping_handler },
  { "/pl", "/pair-local",
    "Pair local, Required args: OTP, target ip, target port",
    pair_local_handler },
  { "/pcl", "/place-call-local",
    "Args: device-id, target ip, target port. Optional protocol arg",
    place_call_local_handler },
  {"/spct", "/set-place-call-timeout",
   "Timeout in seconds, for place-call to complete. Default: 5",
   set_pc_timeout_handler},
  {"/t_dic", "/test_delay_call",
   "Set delay in seconds, for responding to incoming call. Default: 0",
   set_incomingcall_testdelay_handler},
  {"/t_rand", "/test_random_func",
   "Invoke random func, for testing. Prints 32 bytes data as hex.",
   test_random_handler},
  { 0, 0, 0, 0 }
};

static struct cmd basic_commands[] = {
  {"/email", "/email",
   "Set email address in client properties",
   email_handler },
  {"/s", "/status",
   "Display status from mdg lib",
   status_handler },
  {"/who", "/whoami",
   "Print my own device_id in hex",
   whoami_handler },
  {"/op", "/open-for-pairing",
   "Open for pairing, optional timeout as arg, default 120",
   open_for_pairing_handler },
  {"/opf", "/open-for-pairing-preset",
   "Open for pairing, required OTP as arg.",
   open_for_pairing_handler_preset_otp },
  {"/cpm", "/cancel-pairing-mode",
   "Cancel any ongoing pairing", cancel_pairing_mode_handler },
  {"/pair", "/pair-remote",
   "Pair with a peer, Required OTP as arg",
   pair_remote_handler },
  {"/pcr", "/place-call-remote",
   "Connect to a paired peer. Arg: device-id. Optional protocol arg",
   place_call_remote_handler },
  {"/send", "/send-data-to-peer",
   "Send a text message. Arg: connection-id, rest of line is message.",
   send_handler},
  {"/send-hex", "/send-hex-data-to-peer",
   "Send a binary message. Arg: connection-id, rest of line is message.",
   send_hex_handler},
  {"/info", "/info-on-connection",
   "Who is at other end of connection? Arg: connection-id",
   conninfo_handler},
  {"/close", "/close-connection",
   "Close connection (Or pending connection) Arg: connection-id",
   close_conn_handler},
  {"/lp", "/list-pairings",
   "List paired peers, one device_id pr. line",
   list_pairings_handler },
  {"/rmp", "/remove-pairing",
   "Remove pairing, provide device-id as arg",
   remove_pairing_handler },
  {"/h", "/help-basic",
   "Display this help",
   basic_help_handler },
  {"/h2", "/help-advanced",
   "Display help on advanced and testing commands",
   advanced_help_handler },
  {"/q", "/quit",
   "Quit chat client",
   quit_handler },
  { 0, 0, 0, 0 }
};

static void do_help_handler(struct cmd *cmd)
{
  mdg_chat_output_fprintf("Usage:\n");
  mdg_chat_output_fprintf("  In general, commands and their output incl errors show up on stdout.\n"
          "  stdout is for help text and internal debug logging output only.\n"
          "  stdout is for remote control readability by a program.\n================\n"
          "SHORT\tLONG\n");
  for (; cmd->short_name != 0; cmd++) {
    mdg_chat_output_fprintf(" %-9s%-25s%s\n", cmd->short_name, cmd->long_name, cmd->help_text);
  }
}

static void basic_help_handler(char *args_buf, int len)
{
  do_help_handler(basic_commands);
}
static void advanced_help_handler(char *args_buf, int len)
{
  do_help_handler(advanced_commands);
}


static struct cmd* find_command(struct cmd *cmd, char *input)
{
  for (; cmd->short_name != 0; cmd++) {
    if (!strcmp(cmd->short_name, input)) {
      return cmd;
    } else if (!strcmp(cmd->long_name, input)) {
      return cmd;
    }
  }
  return 0;
}

void mdg_chat_client_input(char *in_buf, int len)
{
  struct cmd *cmd;
  int i, args_len;
  char *args;
  for (i = 0; i < len; i++) {
    if (in_buf[i] == ' ') {
      break;
    }
  }
  in_buf[i] = 0; // make cmd a string of it's own.
  i++;
  for ( ; i < len; i++) {
    if (in_buf[i] != ' ') {
      break;
    }
  }
  args = &in_buf[i];
  args_len = len - i;
  args[args_len] = 0; // Teminate args string as well.

  cmd = find_command(basic_commands, in_buf);
  if (cmd == 0) {
    cmd = find_command(advanced_commands, in_buf);
  }
  if (cmd != 0) {
    cmd->handler(args, args_len);
  } else {
    mdg_chat_output_fprintf("Unknown command %s\n", in_buf);
  }
}

static void chatclient_data_received(const uint8_t *data, uint32_t count, uint32_t connection_id)
{
  mdg_chat_output_fprintf("Received data from peer on connection %d: %.*s\n", connection_id, count, data);
  if (mdg_receive_from_peer(connection_id, chatclient_data_received)) {
    mdg_chat_output_fprintf("mdg_receive_from_peer failed\n");
  }
}


// Callbacks invoked by MDG lib.
void mdguser_routing(uint32_t connection_id, mdg_routing_status_t state)
{
  switch (state) {
  case 0:
    mdg_chat_output_fprintf("Callback: mdguser_routing connection_id=%d, state=disconnected\n", connection_id);
    break;
  case 1:
    mdg_chat_output_fprintf("Callback: mdguser_routing connection_id=%d, state=connected\n", connection_id);
    break;
  case -1:
    mdg_chat_output_fprintf("Callback: mdguser_routing connection_id=%d, state=failed\n", connection_id);
    break;
  case -2:
    mdg_chat_output_fprintf("Callback: mdguser_routing connection_id=%d, state=peer_not_available\n", connection_id);
    break;
  default:
    mdg_chat_output_fprintf("Callback: mdguser_routing connection_id=%d, state=%d\n", connection_id, state);
    break;
  }

  if (state == mdg_routing_connected) {
    if (mdg_receive_from_peer(connection_id, chatclient_data_received)) {
      mdg_chat_output_fprintf("mdg_receive_from_peer failed\n");
    }
  }
}

// Callbacks invoked by MDG lib.
int mdgstorage_load_pairing(int pairings_index, uint8_t *peer_id)
{
  load_all_pairings_from_file();
  if (pairings_index < chat_pairings_count) {
    memcpy(peer_id, chat_pairings[pairings_index].peer_id, MDG_PEER_ID_SIZE);
    return MDGSTORAGE_OK;
  }
  return 1;
}

// Callbacks invoked by MDG lib.
int mdgstorage_add_pairing(uint8_t *peer_id)
{
  int i;
  load_all_pairings_from_file();
  for (i = 0; i < chat_pairings_count; ++i) {
    if (!memcmp(&chat_pairings[i].peer_id, peer_id, MDG_PEER_ID_SIZE)) {
      return MDGSTORAGE_OK; // Existing pairing matched.
    }
  }
  memcpy(&chat_pairings[chat_pairings_count].peer_id, peer_id, MDG_PEER_ID_SIZE);
  chat_pairings_count++;
  save_all_pairings_to_file();
  return MDGSTORAGE_OK;
}

// Callbacks invoked by MDG lib.
int mdgstorage_remove_pairing(unsigned char *peer_id)
{
  int i = 0, j;
  load_all_pairings_from_file();
  while (i < chat_pairings_count) {
    if (!memcmp(&chat_pairings[i].peer_id, peer_id, MDG_PEER_ID_SIZE)) {
      j = i+1;
      if (j < chat_pairings_count) {
        // overwrite pairing with remaining pairings.
        while (j < chat_pairings_count) {
          memcpy(&chat_pairings[j-1], &chat_pairings[j], sizeof(pairing_t));
        }
      } else {
        // Wipe pairing.
        memset(&chat_pairings[i], 0, sizeof(pairing_t));
        i++;
        if (i == chat_pairings_count) {
          chat_pairings_count--;
        }
      }
    } else {
      i++;
    }
  }

  save_all_pairings_to_file();
  return MDGSTORAGE_OK;
}

// Callbacks invoked by MDG lib.
int32_t mdguser_incoming_call(const char *protocol)
{
  mdg_chat_output_fprintf("Got incoming call for protocol %s\n", protocol);
  if (incomingcall_testdelay > 0) {
    mdg_chat_output_fprintf("Delaying response for incoming call for %d seconds\n", incomingcall_testdelay);
#if __linux
	sleep(incomingcall_testdelay);
#elif MDG_WINDOWS
	Sleep(incomingcall_testdelay * 1000);
#else
	mdg_chat_output_fprintf("The response for incoming call could not be delayed as sleep is not supported on the current platform.\n");//TODO
	return 0;
#endif
    mdg_chat_output_fprintf("Delaying response for incoming call for %d seconds completed.\n", incomingcall_testdelay);
  }
  return 0; // Accept call.
}

// Callbacks invoked by MDG lib.
int mdgstorage_load_preset_pairing_token(char *printedLabelOtp)
{
  if (preset_otp_buffer[0] != 0) {
    int otp_len = strlen(preset_otp_buffer);
    if (otp_len < MDG_PRESET_PAIR_ID_SIZE) {
      mdg_chat_output_fprintf("mdgstorage_load_preset_pairing_token: "
                              "using preset OTP og len %d\n", otp_len);
      memcpy(printedLabelOtp, preset_otp_buffer, otp_len);
      printedLabelOtp[otp_len] = 0;
    } else {
      // Pre-def otp too long. Fail.
      return 1;
    }
  } else {
      mdg_chat_output_fprintf("mdgstorage_load_preset_pairing_token: "
                              "using random OTP\n");
    printedLabelOtp[0] = 0;
  }
  return 0;
}

// Callbacks invoked by MDG lib.
int mdgstorage_load_license_key(void *license_key)
{
  char *lk = (char*)license_key;
  int i;
  // Make the license key an (invalid) test sequence, for easy recognition.
  for (i = 0; i < MDG_LICENSEKEY_SIZE; i++) {
    lk[i] = i;
  }
  return 0;
}

// Load/create+store private key for this instance in local filesystem.
void chatclient_load_or_create_private_key(uint8_t *pk)
{
  FILE *f = fopen("chat_demo_private.key", "rb");
  int s = 0;
  if (f != NULL) {
    s = fread(pk, 1, MDG_PEER_ID_SIZE, f);
    fclose(f);
  }
  if (s != MDG_PEER_ID_SIZE) {
    mdg_make_private_key(pk);
    FILE *f = fopen("chat_demo_private.key", "wb");
    if (f != NULL) {
      s = fwrite(pk, 1, MDG_PEER_ID_SIZE, f);
      fclose(f);
    }
  }
}
