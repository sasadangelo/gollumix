extern void keyboard_init(void);

/*
 * This function initializes the console driver. A console is composed by a
 * keyboard and video.
 */
void con_init(void) {
	// initialize the keyboard driver
	keyboard_init();
}
