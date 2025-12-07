#include"menuScene.h"
#include"seeScene.h"
#include"music.h"
USING_NS_CC;
using namespace ui;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in seeScene.cpp\n");
}


