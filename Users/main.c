#include "sys.h"
#include "stmflash.h"

typedef void (*APP_FUNC)();
APP_FUNC jumpapp;


void JumpToApp(uint32_t app_address)
{
	if(((*(__IO uint32_t*)app_address) & 0x2FFE0000) == 0x20000000)
	{
		volatile uint32_t JumpAPPaddr;
		if(((*(uint32_t*)app_address) & 0x2FFE0000) == 0x20000000)
		{
			JumpAPPaddr = *(volatile uint32_t*)(app_address + 4);
			jumpapp = (APP_FUNC) JumpAPPaddr;
			__set_MSP(*(__IO uint32_t*) app_address);
			jumpapp();
		}
	}
}


int main(void)
{
  //HAL_Init();                                                 /* 初始化HAL库 */

  NVIC_Configuration();

	while(1)
	{
		if(ReadFlashTest(UPFLAGADDR) == 0x55555555)  //升级标志
		{
				update_firmware(APP1ADDR,APP2ADDR);//固件升级操作
				EarseFlash_1K(UPFLAGADDR);//擦除升级标志位
				WriteFlash(UPFLAGADDR,nbuff_AAAA,4);//写入AAAAAAAA
				NVIC_SystemReset();//系统复位
		}
		else 
		{
				JumpToApp(APP1ADDR);
		}
	}  
	
}
