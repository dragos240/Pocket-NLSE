#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "ui/list.h"
#include "ui/ui.h"
#include "core/linkedlist.h"
#include "actions/simple.h"
#include "core/constants.h"
#include "core/common.h"
#include "ui/error.h"
#include "ui/menu.h"
#include "ui/list.h"
#include "ui/prompt.h"

void bankMenu_open(list_item* selected){
	prompt_display("Bank Menu", (const char*)selected->name, COLOR_BLACK, false, NULL, NULL, NULL);
}
