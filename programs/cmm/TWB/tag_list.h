enum {DT=1, OL, UL};
struct LIST
{
	int level;
	int ordered[5];
	int counter[5];
	void reset();
	void upd_level();
	int inc_counter();
	int get_order_type();
};

void LIST::reset()
{
	level = 0;
}

void LIST::upd_level(int direction, type)
{
	if (direction == 1) && (level<5) {
		level++;
		counter[level]=0;
		ordered[level]=type;
	}
	if (direction == 0) && (level>0) {
		level--;
	}
}

int LIST::inc_counter()
{
	counter[level]++;
	return counter[level];
}

int LIST::get_order_type()
{
	return ordered[level];
}
