int cap(int x) {
	if (x < 30) {
		if (x > -30) return 0;
		if (x < -127) return -127;
	}
	if (x > 127) return 127;

	return x;
}
