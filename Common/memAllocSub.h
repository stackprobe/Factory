void *memCalloc(uint size);
void *memCloneWithBuff(void *block, uint size, uint buffsize);
void *memClone(void *block, uint size);

#define na(TYPE_T, num) \
	((TYPE_T *)memCalloc(sizeof(TYPE_T) * (num)))

#define nb(TYPE_T) \
	(na(TYPE_T, 1))
