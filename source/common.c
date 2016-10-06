#include <3ds.h>

bool is3dsx;

void setIs3dsx(){
	u64 id;
	APT_GetProgramID(&id);

	if(id == 0x000400000cce2e00){
		is3dsx = false;
		return;
	}

	is3dsx = true;
}
