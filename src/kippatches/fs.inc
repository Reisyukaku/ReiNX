static kipdiff_t fs_diffs_100_nosigchk[4] = {
  { 0x19468, 4, "\x08\x01\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x194A0, 4, "\xBA\x09\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x3A79C, 4, "\xE0\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_100_nocmac[5] = {
  { 0x45C78, 16, "\x29\x01\x0C\x0A\xEA\x17\x9F\x1A\x1F\x01\x04\xF1\x29\x01\x0A\x0A", "\x1F\x20\x03\xD5\xEA\x17\x9F\x1A\x1F\x01\x04\xF1\x1F\x20\x03\xD5" },
  { 0x46488, 8, "\x29\x01\x0D\x0A\x29\x01\x0F\x0A", "\x1F\x20\x03\xD5\x1F\x20\x03\xD5" },
  { 0x46498, 8, "\x29\x01\x0B\x0A\x8B\x09\x40\x39", "\x1F\x20\x03\xD5\x8B\x09\x40\x39" },
  { 0x464A8, 8, "\x1F\x01\x04\xF1\x29\x01\x0A\x0A", "\x1F\x01\x04\xF1\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_200_nosigchk[4] = {
  { 0x15D5C, 4, "\x0A\x01\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x15DF4, 4, "\xBC\x0A\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x3F720, 4, "\x00\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_200_nocmac[6] = {
  { 0x4E744, 4, "\x29\x01\x0C\x0A", "\x1F\x20\x03\xD5" },
  { 0x4E750, 4, "\x29\x01\x0A\x0A", "\x1F\x20\x03\xD5" },
  { 0x4EF50, 8, "\x29\x01\x0D\x0A\x29\x01\x0F\x0A", "\x1F\x20\x03\xD5\x1F\x20\x03\xD5" },
  { 0x4EF5C, 4, "\x29\x01\x0B\x0A", "\x1F\x20\x03\xD5" },
  { 0x4EF70, 4, "\x29\x01\x0A\x0A", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_210_nosigchk[4] = {
  { 0x15ECC, 4, "\x0A\x01\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x15F64, 4, "\xDF\x0A\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x3FAF8, 4, "\x00\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_210_nocmac[6] = {
  { 0x4EB24, 4, "\x29\x01\x0C\x0A", "\x1F\x20\x03\xD5" },
  { 0x4EB30, 4, "\x29\x01\x0A\x0A", "\x1F\x20\x03\xD5" },
  { 0x4F330, 8, "\x29\x01\x0D\x0A\x29\x01\x0F\x0A", "\x1F\x20\x03\xD5\x1F\x20\x03\xD5" },
  { 0x4F33C, 4, "\x29\x01\x0B\x0A", "\x1F\x20\x03\xD5" },
  { 0x4F350, 4, "\x29\x01\x0A\x0A", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_300_nosigchk[4] = {
  { 0x18D8C, 4, "\x0A\x01\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x18E24, 4, "\x52\x0C\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x49EC8, 4, "\x40\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_300_nocmac[6] = {
  { 0x5B1D8, 4, "\x80\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5B6D4, 4, "\x00\x02\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5F72C, 4, "\x80\x14\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5FF40, 4, "\x00\x08\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x600B8, 4, "\xE0\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_301_nosigchk[4] = {
  { 0x18DF8, 4, "\x0A\x01\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x18E90, 4, "\x52\x0C\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x49F34, 4, "\xE0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_301_nocmac[6] = {
  { 0x5B294, 4, "\x80\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5B790, 4, "\x00\x02\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5F7E8, 4, "\x80\x14\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x5FFFC, 4, "\x00\x08\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x60174, 4, "\xE0\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_401_nosigchk[4] = {
  { 0x1C464, 4, "\x46\xEE\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x1C4FC, 4, "\x3C\x2F\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x57934, 4, "\xE0\x02\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_401_nocmac[6] = {
  { 0x6C540, 4, "\xC0\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x6CA54, 4, "\x40\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x715B0, 4, "\xA0\x0C\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x71DCC, 4, "\xA0\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x76300, 4, "\x00\x05\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_401_nogc[3] = {
  { 0xA3458, 4, "\x14\x40\x80\x72", "\x14\x80\x80\x72" },
  { 0xAAB44, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_410_nogc[3] = {
  { 0xA34BC, 4, "\x14\x40\x80\x72", "\x14\x80\x80\x72" },
  { 0xAABA8, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_500_nosigchk[4] = {
  { 0x22D68, 4, "\x9A\xE3\xFF\x97 ","\xE0\x03\x1F\x2A" },
  { 0x22DDC, 4, "\x7D\x3E\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x7D490, 4, "\x40\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_500_nocmac[8] = {
  { 0x8F478, 4, "\x80\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x8F988, 4, "\xA0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x94F94, 4, "\x60\x0C\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x958DC, 4, "\x60\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x95A1C, 4, "\x20\x07\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x9C6C8, 4, "\xC0\x05\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x9C7F4, 4, "\xC0\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_500_nogc[3] = {
  { 0xCF3C4, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0xD73A0, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_510_nosigchk[4] = {
  { 0x22D98, 4, "\x9A\xE3\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x22E0C, 4, "\x85\x3E\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0x7D860, 4, "\x40\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_510_nocmac[8] = {
  { 0x8F848, 4, "\x80\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x8FD58, 4, "\xA0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x95364, 4, "\x60\x0C\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x95CAC, 4, "\x60\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x95DEC, 4, "\x20\x07\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x9CA98, 4, "\xC0\x05\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x9CBC4, 4, "\xC0\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_600_exfat_nocmac[8] = {
  { 0x10B7F4, 4, "\x20\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x10BCF8, 4, "\xA0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x113934, 4, "\x60\x14\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x11429C, 4, "\x60\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x1143DC, 4, "\x20\x07\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x11B128, 4, "\xC0\x05\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x11B254, 4, "\xC0\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

//exfat offsets are simply shifted by + 0xB700
static kipdiff_t fs_diffs_600_nocmac[8] = {
  { 0x1000F4, 4, "\x20\x0A\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x1005F8, 4, "\xA0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x108234, 4, "\x60\x14\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x108B9C, 4, "\x60\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x108CDC, 4, "\x20\x07\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x10FA28, 4, "\xC0\x05\x00\x36", "\x1F\x20\x03\xD5" },
  { 0x10FB54, 4, "\xC0\x06\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_510_nogc[3] = {
  { 0xCF794, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0xD7770, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_600_exfat_nogc[3] = {
  { 0x15EFF4, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0x138320, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_600_nogc[3] = {
  { 0x1538F4, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0x12CC20, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_600_exfat_nosigchk[4] = {
  { 0x7C934, 4, "\x07\xDF\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x7C9A8, 4, "\x8E\x3E\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xF678C, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_600_nosigchk[4] = {
  { 0x71234, 4, "\x07\xDF\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x712A8, 4, "\x8E\x3E\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xEB08C, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_700_exfat_nogc[3] = {
  { 0x1674B4, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0x13F710, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_700_nogc[3] = {
  { 0x15BF04, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0x134160, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_700_exfat_nosigchk[4] = {
  { 0x7FFA4, 4, "\x33\xD7\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x7FFDC, 4, "\x31\x43\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xFDB94, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_700_nosigchk[4] = {
  { 0x7C934, 4, "\x33\xD7\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x74A2C, 4, "\x31\x43\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xF25E4, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_800_nosigchk[4] = {
  { 0x762D4, 4, "\xBB\xD6\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x7630C, 4, "\x51\x44\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xF49A4, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_800_exfat_nosigchk[4] = {
  { 0x81884, 4, "\xBB\xD6\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x818BC, 4, "\x51\x44\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xFFF54, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_800_nogc[3] = {
  { 0x136800, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0x15EB94, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_800_exfat_nogc[3] = {
  { 0x141DB0, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0x16A144, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_810_nosigchk[4] = {
  { 0x762D4, 4, "\xBB\xD6\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x7630C, 4, "\x51\x44\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xF49A4, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_810_exfat_nosigchk[4] = {
  { 0x81884, 4, "\xBB\xD6\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x818BC, 4, "\x51\x44\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xFFF54, 4, "\xC0\x03\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL },
};

static kipdiff_t fs_diffs_810_nogc[3] = {
  { 0x136800, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0x15EB94, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_810_exfat_nogc[3] = {
  { 0x141DB0, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0x16A144, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_900_nosigchk[4] = {
  { 0x61E54, 4, "\x23\xD7\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x61E8C, 4, "\x4D\x31\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xCA040, 4, "\x40\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_900_nogc[3] = {
  { 0x129420, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
  { 0x143268, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_910_nogc[3] = 
{
	{ 0x129430, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
	{ 0x143278, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
	{ 0, 0, NULL, NULL }
};

static kipdiff_t fs_diffs_910_nosigchk[4] = {
  { 0x61E64, 4, "\x1F\xD7\xFF\x97", "\xE0\x03\x1F\x2A" },
  { 0x61E9C, 4, "\x4D\x31\x00\x94", "\xE0\x03\x1F\x2A" },
  { 0xCA050, 4, "\x40\x04\x00\x36", "\x1F\x20\x03\xD5" },
  { 0, 0, NULL, NULL }
};

static kipdiff_t fs_nogc_1000[] = 
{
	{ 0x13BE90, 8, "\xF4\x4F\xBE\xA9\xFD\x7B\x01\xA9", "\xE0\x03\x1F\x2A\xC0\x03\x5F\xD6" },
	{ 0x14DE08, 4, "\x14\x40\x80\x52", "\x14\x80\x80\x52" },
	{ 0, 0, NULL, NULL }
};

static kipdiff_t fs_sigpatch_1000[] =
{
	{ 0xDC01C, 8, "\x75\x2E\xFE\x97\x40\x04\x00\x36", "\x20\x00\x80\xD2\x1F\x20\x03\xD5" },
	{ 0, 0, NULL, NULL }
};

/* patches */

static kippatch_t fs_kip_patches_100[] = {
  { "nosigchk", fs_diffs_100_nosigchk },
  { "nocmac", fs_diffs_100_nocmac },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_200[] = {
  { "nosigchk", fs_diffs_200_nosigchk },
  { "nocmac", fs_diffs_200_nocmac },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_210[] = {
  { "nosigchk", fs_diffs_210_nosigchk },
  { "nocmac", fs_diffs_210_nocmac },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_300[] = {
  { "nosigchk", fs_diffs_300_nosigchk },
  { "nocmac", fs_diffs_300_nocmac },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_301[] = {
  { "nosigchk", fs_diffs_301_nosigchk },
  { "nocmac", fs_diffs_301_nocmac },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_401[] = {
  { "nosigchk", fs_diffs_401_nosigchk },
  { "nocmac", fs_diffs_401_nocmac },
  { "nogc", fs_diffs_401_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_410[] = {
  { "nosigchk", fs_diffs_401_nosigchk },
  { "nocmac", fs_diffs_401_nocmac },
  { "nogc", fs_diffs_410_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_500[] = {
  { "nosigchk", fs_diffs_500_nosigchk },
  { "nocmac", fs_diffs_500_nocmac },
  { "nogc", fs_diffs_500_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_510[] = {
  { "nosigchk", fs_diffs_510_nosigchk },
  { "nocmac", fs_diffs_510_nocmac },
  { "nogc", fs_diffs_510_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_600_40[] = {
  { "nosigchk", fs_diffs_600_nosigchk }
,  { "nocmac", fs_diffs_600_nocmac },
  { "nogc", fs_diffs_600_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_600_40_exfat[] = {
  { "nosigchk", fs_diffs_600_exfat_nosigchk },
  { "nocmac", fs_diffs_600_exfat_nocmac },
  { "nogc", fs_diffs_600_exfat_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_600_50[] = {
  { "nosigchk", fs_diffs_600_nosigchk },
  { "nocmac", fs_diffs_600_nocmac },
  { "nogc", fs_diffs_600_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_600_50_exfat[] = {
  { "nosigchk", fs_diffs_600_exfat_nosigchk },
  { "nocmac", fs_diffs_600_exfat_nocmac },
  { "nogc", fs_diffs_600_exfat_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_700[] = {
  { "nosigchk", fs_diffs_700_nosigchk },
  { "nogc", fs_diffs_700_nogc },
  { NULL, NULL }
};


static kippatch_t fs_kip_patches_700_exfat[] = {
  { "nosigchk", fs_diffs_700_exfat_nosigchk },
  { "nogc", fs_diffs_700_exfat_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_800[] = {
  { "nosigchk", fs_diffs_800_nosigchk },
  { "nogc", fs_diffs_800_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_800_exfat[] = {
  { "nosigchk", fs_diffs_800_exfat_nosigchk },
  { "nogc", fs_diffs_800_exfat_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_810[] = {
  { "nosigchk", fs_diffs_810_nosigchk },
  { "nogc", fs_diffs_810_nogc},
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_810_exfat[] = {
  { "nosigchk", fs_diffs_810_exfat_nosigchk },
  { "nogc", fs_diffs_810_exfat_nogc },
  { NULL, NULL }
};

static kippatch_t fs_kip_patches_900[] = {
  { "nosigchk", fs_diffs_900_nosigchk },
  { "nogc", fs_diffs_900_nogc },
  { NULL, NULL}
};

static kippatch_t fs_kip_patches_910[] = {
  { "nosigchk", fs_diffs_910_nosigchk },
  { "nogc", fs_diffs_910_nogc },
  { NULL, NULL}
};

static kippatch_t fs_kip_patches_1000[] = {
  { "nogc", fs_nogc_1000 },
  { "nosigchk", fs_sigpatch_1000 },
  { NULL, NULL}
};
