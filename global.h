#pragma once
#include "history.h"
#include <semaphore.h>
//Global variables

extern FileHistory* g_history;
extern sem_t* history_sem;
