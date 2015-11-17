void start_kernel(void) {
	unsigned char *vid_mem = (unsigned char *)(0xb8000);

	*vid_mem++ = 'H';
	*vid_mem++ = 0x6;
	*vid_mem++ = 'e';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'l';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'l';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'o';
    *vid_mem++ = 0x6;
    *vid_mem++ = ' ';
	*vid_mem++ = 0x6;
	*vid_mem++ = 'K';
	*vid_mem++ = 0x6;
	*vid_mem++ = 'e';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'r';
    *vid_mem++ = 0x6;
    *vid_mem++ = 'n';
    *vid_mem++ = 0x6;
	*vid_mem++ = 'e';
	*vid_mem++ = 0x6;
	*vid_mem++ = 'l';
	*vid_mem++ = 0x6;

	while(1);
}
