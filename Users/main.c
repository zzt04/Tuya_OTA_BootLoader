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
  //HAL_Init();                                                 /* ��ʼ��HAL�� */

  NVIC_Configuration();

	while(1)
	{
		if(ReadFlashTest(UPFLAGADDR) == 0x55555555)  //������־
		{
				update_firmware(APP1ADDR,APP2ADDR);//�̼���������
				EarseFlash_1K(UPFLAGADDR);//����������־λ
				WriteFlash(UPFLAGADDR,nbuff_AAAA,4);//д��AAAAAAAA
				NVIC_SystemReset();//ϵͳ��λ
		}
		else 
		{
				JumpToApp(APP1ADDR);
		}
	}  
	
}
