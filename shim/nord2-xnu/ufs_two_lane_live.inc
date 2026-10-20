static bool
nord2_live_arg_exact(const char *key, const char *expected, unsigned *occurrences)
{
	/* PE_boot_args() already returns CommandLine, not the boot_args struct. */
	const char *cmd = PE_boot_args();
	const size_t capacity = sizeof(((struct boot_args *)0)->CommandLine);
	*occurrences = 0;
	if (!cmd) { *occurrences = 1; return false; }
	size_t length = 0, key_length = 0;
	while (length < capacity && cmd[length]) { length++; }
	if (length == capacity) { *occurrences = 1; return false; }
	while (key[key_length]) { key_length++; }
	bool matched = false;
	for (size_t pos = 0; pos < length;) {
		/* ASCII whitespace, as used by the offline image argument split. */
		if (cmd[pos] == ' ' || (cmd[pos] >= '\t' && cmd[pos] <= '\r')) { pos++; continue; }
		size_t begin = pos;
		while (pos < length && cmd[pos] != ' ' && !(cmd[pos] >= '\t' && cmd[pos] <= '\r')) { pos++; }
		size_t end = pos, n = 0;
		while (n < key_length && begin + n < end && cmd[begin + n] == key[n]) { n++; }
		if (n != key_length || (begin + n != end && cmd[begin + n] != '=')) { continue; }
		(*occurrences)++;
		if (begin + n == end) { continue; } /* bare flag is never explicit */
		size_t value = begin + n + 1;
		n = 0;
		while (value + n < end && expected[n] && cmd[value + n] == expected[n]) { n++; }
		if (value + n == end && !expected[n]) { matched = true; }
	}
	return *occurrences == 1 && matched;
}

static bool nord2_live_armed,nord2_live_invalid,nord2_live_active;
static void nord2_live_arm(void)
{
 if(nord2_live_armed)return;
 unsigned count=0;
 bool exact=nord2_live_arg_exact("nord2ufs2live","1",&count);
 if(!count)return;
 unsigned zero_count=0;
 if(count==1 && nord2_live_arg_exact("nord2ufs2live","0",&zero_count))return;
 nord2_live_armed=true; /* permanent before allocation, map, ownership or SET */
 nord2_live_invalid=!exact;
 static const char *const required[][2]={
  {"nord2ufs2live","1"},{"nord2stopat","23"},{"cpus","8"},
  {"rd","ufsroot0"},{"ufsroot","userdata"},{"ufsrw","1"},{"rootrw","1"},
  {"nord2ufsprofile","1"}
 };
 for(unsigned i=0;i<sizeof(required)/sizeof(required[0]);i++)
  if(!nord2_live_arg_exact(required[i][0],required[i][1],&count))nord2_live_invalid=true;
 (void)nord2_live_arg_exact("nord2ufs2lane","1",&count);
 if(count)nord2_live_invalid=true; /* stop-mode token must be absent, even0 */
 printf("NORD2 UFS X2 LIVE POLICY: armed=1 invalid=%u return_blocked=1 reason=manual-reset-required\n",(unsigned)nord2_live_invalid);
}
static void nord2_live_early_halt(int error) __attribute__((noreturn));
static void nord2_live_early_halt(int error)
{
 printf("NORD2 UFS X2 LIVE REFUSED: error=%d root_allowed=0 return_blocked=1 manual_reset=1\n",error);
 for(;;){delay(1000000);}
}
