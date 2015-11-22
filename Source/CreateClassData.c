#include "CreateClassData.h"
#include "BitmapLoader.h"
#include "STATS.H"
#include "GRAPHICS.H"
#include "MEMORY.H"

ItemListEntry *FindFromItemList(char *itemname)
{
	int i = 0;
	ItemListEntry *retvalue;

	DebugRoutine("FindFromItemList");

	retvalue = NULL;
	for (i = 0; i < ItemListCount; i++)
	{
		if (&ItemList[i] == NULL)
			continue;

		if (strcmp(itemname, ItemList[i].name) == 0)
			retvalue = &ItemList[i];
	}

	DebugEnd();

	return retvalue;
}

T_void LoadItemList()
{
	FILE *fp;
	char filename[32];
	char cbuffer = ' ';
	char name[128];
	float num = 0;
	int lines = 0;
	int i = 0;
	ItemListEntry *item;

	DebugRoutine ("LoadItemList");

	sprintf(filename, "./classes/itemlist.dat");
	if (fp = fopen(filename, "r"))
	{
		//count lines in order to allocate space
		while (!feof(fp))
		{
			fscanf(fp, "%c", &cbuffer);
			if (cbuffer == 10)
			{
				lines++;
			}
		}

		//size array
		// Always assume there's 1 extra entry because eof will terminate the last line
		ItemListCount = lines + 1;

		rewind(fp);
		//count lines in order to assign space
		cbuffer = ' ';
		while (!feof(fp))
		{
			//read values
			fscanf(fp, "%s", &name);
			//skip tab
			fseek(fp, 1, SEEK_CUR);
			fscanf(fp, "%f", &num);

			//create new item
			item = (ItemListEntry*)MemAlloc(sizeof(ItemListEntry));
			strcpy(item->name, name);
			item->num = (T_word16)num;
			//add to list
			ItemList[i] = *item;

			i++;

			//read next char data
			fscanf(fp, "%c", &cbuffer);
		}	

		fclose(fp);
	}

	fp = NULL;
	item = NULL;

	DebugEnd();
}

T_void InitCreateClassData (T_byte8 classnum)
{
	CreateClassData *classdata;
    DebugRoutine ("InitCreateClassData");

	classdata = &CreateClassDatas[classnum];
	classdata->ClassNum = classnum;
	classdata->RegenHealthModifier = 1;
	classdata->RegenManaModifier = 1;
	classdata->JumpModifier = 1;
	classdata->DamageModifier = 1;
	classdata->ThiefModifier = 0;
	classdata->BasePunchDamage = 3;
	classdata->SpellSystem = SKILL_SYSTEM_MAGIC_MAGE;
	
	classdata = NULL;

	DebugEnd();
}

char* FindClassImage(T_byte8 classnum)
{
	char* filename = malloc(MAXPICFILENAMELENGTH);
	char stmp[MAXPICFILENAMELENGTH];

    DebugRoutine ("FindClassImage");

	sprintf(stmp, "UI/CREATEC/CHAR%02d", classnum);
    if (PictureExist (stmp))
    {
	}
	else
	{
		sprintf(stmp, "classes/%i.png", (int)classnum);
	}

	strcpy(filename, stmp);

	DebugEnd();
	return filename;
}

T_void ReadClassData(T_byte8 classnum)
{
	FILE *fp;
	char filename[32];
	char cbuffer = ' ';
	char sbuffer[MAXLEVELTITLELENGTH] = "";
	char title[MAXCLASSTITLELENGTH];
	char descr[MAXCLASSDESCRLENGTH] = "";
	char levelTitles[NUM_TITLES_PER_CLASS][MAXLEVELTITLELENGTH];
	char itemname[128];
	float itemcount = 0;
	long temppos = 0;
	int itemscount = 0;
	StartingItemData *item;
	int attr[NUM_ATTRIBUTES];
	int adv[NUM_ATTRIBUTES];
	int i, j;
	int magic, canUseFlag;
	char* picturePath;
	float num;
	float  hmod, manamod, jmod, dmod, movemod, tmod, pdmg;

	DebugRoutine ("ReadClassData");

	sprintf(filename, "./classes/%i.dat", (int)classnum);
	if (fp = fopen(filename, "r"))
	{
		//title
		fscanf(fp, "%s", title);

		//description
		while(cbuffer != '~')
		{
			fscanf(fp, "%c", &cbuffer);

			//don't write delmiters
			if (cbuffer != '~')
				descr[strlen(descr)] = cbuffer;
		}
		fseek(fp, 2, SEEK_CUR);
		
		//attributes
		for (i = 0; i<NUM_ATTRIBUTES;i++)
		{
			fscanf(fp, "%f", &num);
			attr[i] = (int)num;
			fscanf(fp, "%c", &cbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//advancement
		for (i = 0; i<NUM_ATTRIBUTES;i++)
		{
			fscanf(fp, "%f", &num);
			adv[i] = (int)num;
			fscanf(fp, "%c", &cbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//level titles
		for (i = 0; i<NUM_TITLES_PER_CLASS; i++)
		{
			cbuffer = ' ';
			j = 0;
			while(cbuffer != ',' && cbuffer != '~')
			{
				fscanf(fp, "%c", &cbuffer);

				//don't write delmiters
				if (cbuffer != ',' && cbuffer != '~')
					sbuffer[j] = cbuffer;
				//end string with null val
				else
					sbuffer[j] = '\0';
				j++;
			}
			strcpy(levelTitles[i], sbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//Magic System
		fscanf(fp, "%f", &num);
		magic = (int)num;
		fseek(fp, 1, SEEK_CUR);

		//CanUseBy Flag
		fscanf(fp, "%f", &num);
		canUseFlag = (int)num;
		if (canUseFlag == 0)
			canUseFlag = 1<<classnum;
		fseek(fp, 3, SEEK_CUR);

		//Health Regen Mod
		fscanf(fp, "%f", &num);
		hmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Mana Regen Mod
		fscanf(fp, "%f", &num);
		manamod = num;
		fseek(fp, 1, SEEK_CUR);

		//Jump Mod
		fscanf(fp, "%f", &num);
		jmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Damage Mod
		fscanf(fp, "%f", &num);
		dmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Move Mod
		fscanf(fp, "%f", &num);
		movemod = num;
		fseek(fp, 1, SEEK_CUR);

		//Thief Mod
		fscanf(fp, "%f", &num);
		tmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Base Punch Damage
		fscanf(fp, "%f", &num);
		pdmg = num;
		fseek(fp, 3, SEEK_CUR);

		//canuse weapons
		for (i = 0; i < (EQUIP_WEAPON_TYPE_UNKNOWN); i++)
		{
			fscanf(fp, "%f", &num);
			CreateClassDatas[classnum].CanUseWeapon[i] = (E_Boolean)num;
			fseek(fp, 1, SEEK_CUR);
		}
		fseek(fp, 2, SEEK_CUR);

		//canuse armor
		for (i = 0; i < NUM_ARMOR_TYPES; i++)
		{
			fscanf(fp, "%f", &num);
			CreateClassDatas[classnum].CanUseArmor[i] = (E_Boolean)num;
			fseek(fp, 1, SEEK_CUR);
		}
		fseek(fp, 2, SEEK_CUR);

		//Starting Items List
		{
			//get total number of weapons to be used
			temppos = ftell(fp);
			cbuffer = ' ';
			while(cbuffer != '~')
			{
				fscanf(fp, "%c", &cbuffer);
				if (cbuffer == ',')
				{
					itemscount++;
				}
			}

			//start item list
			CreateClassDatas[classnum].StartingItemsCount = itemscount + 1;
			CreateClassDatas[classnum].StartingItems = (StartingItemData*)MemAlloc(sizeof(StartingItemData) * CreateClassDatas[classnum].StartingItemsCount);

			//rewind
			fseek(fp, temppos, SEEK_SET);

			//read in numbers
			cbuffer = ' ';
			itemscount = 0;
			while(cbuffer != '~')
			{
				//read values
				fscanf(fp, "%s", &itemname);
				fscanf(fp, "%f", &itemcount);

				//create new item entry
				item = (StartingItemData*)MemAlloc(sizeof(StartingItemData));
				item->num = FindFromItemList(itemname)->num;
				item->count = (T_byte8)itemcount;

				//assign entry
				CreateClassDatas[classnum].StartingItems[itemscount] = *item;

				fscanf(fp, "%c", &cbuffer);
				//if another entry follows
				if (cbuffer == ',')
				{
					//skip newline char
					fseek(fp, 2, SEEK_CUR);
					itemscount++;
				}
			}
		}

		fclose(fp);
	}

	strcpy(CreateClassDatas[classnum].Title, title);
	strcpy(CreateClassDatas[classnum].Descr, descr);
	for (i=0; i<NUM_ATTRIBUTES;i++)
	{
		CreateClassDatas[classnum].Attributes[i] = attr[i];
		CreateClassDatas[classnum].Advancement[i] = adv[i];
	}
	for (i=0; i<NUM_TITLES_PER_CLASS;i++)
	{
		strcpy(CreateClassDatas[classnum].LevelTitles[i], levelTitles[i]);
	}
	CreateClassDatas[classnum].SpellSystem = magic;
	CreateClassDatas[classnum].CanUseFlag = canUseFlag;
	CreateClassDatas[classnum].RegenHealthModifier = hmod;
	CreateClassDatas[classnum].RegenManaModifier = manamod;
	CreateClassDatas[classnum].JumpModifier = jmod;
	CreateClassDatas[classnum].DamageModifier = dmod;
	CreateClassDatas[classnum].MoveModifier = movemod;
	CreateClassDatas[classnum].ThiefModifier = tmod;
	CreateClassDatas[classnum].BasePunchDamage = (int)pdmg;

	picturePath = FindClassImage(classnum);

	CreateClassDatas[classnum].PicturePath = picturePath;

	fp=NULL;
	item=NULL;

	DebugEnd();
}

T_void LoadCreateClassDatas(T_void)
{
	T_byte8 i;

	DebugRoutine("LoadCreateClassDatas");

	LoadItemList();

	for (i = 0; i < NUM_CLASSES; i++)
	{	
		//create and assign values
		InitCreateClassData(i);
		ReadClassData(i);		
	}

	DebugEnd();
}

T_void DestroyCreateClassDatas(T_void)
{
	DebugRoutine("DestroyCreateClassDatas");

	for (int i = 0; i < CLASS_UNKNOWN; i++)
	{
		free(CreateClassDatas[i].PicturePath);
	}


	DebugEnd();
}