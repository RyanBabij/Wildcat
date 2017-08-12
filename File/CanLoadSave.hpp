#ifndef CANLOADSAVE_HPP
#define CANLOADSAVE_HPP

//See DataTools::toCharArray

class CanLoadSave
{
	public:
	
	CanLoadSave() {}
	virtual bool load(unsigned char*) { return false; }
	virtual unsigned char *save() { return 0; }
	virtual unsigned int getObjectSize() { return 0; }
	~CanLoadSave() {}
	
};

#endif
