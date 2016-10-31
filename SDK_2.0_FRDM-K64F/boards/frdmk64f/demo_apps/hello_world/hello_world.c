/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#if defined(__CC_ARM)
#include "time.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

lua_State *L;

const char* testfunc = "print(add2(1.0,2.0)) print(sub2(20.1,19))";
//const char* testfunc = "led_on()";

#if 0
const char LUA_SCRIPT_GLOBAL[] = {  \
  "off = 10\n"
  "on = 10\n"
  "led_off()\n"
  "delay(off)\n"
  "led_on()\n"
  "delay(on)\n"
};

#else

const char LUA_SCRIPT_GLOBAL[] ="  \
off = 10     \
on = 10       \
while 1 do \
 led_off() \
 delay(off)    \
 led_on()        \
 delay(on)      \
 off= off * 1.02      \
 on= on * 1.02      \
  ifoff > 800 then off = 500 end \
  ifon > 800 then on = 400 end \
end";
#endif

#if 1
const char lua_test[] = {   
    "print(\"Hello,I am lua!\\n--this is newline printf\")\n"  
    "function foo()\n"  
    "  local i = 0\n"  
    "  local sum = 1\n"  
    "  while i <= 10 do\n"  
    "    sum = sum * 2\n"  
    "    i = i + 1\n"  
    "  end\n"  
    "return sum\n"  
    "end\n"  
    "print(\"sum =\", foo())\n"  
    "print(\"and sum = 2^11 =\", 2 ^ 11)\n"  
    "print(\"exp(200) =\", math.exp(200))\n"  
};  
#endif


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static int lua_led_on(lua_State *L);
static int lua_led_off(lua_State *L);
static int lua_delay(lua_State *L);
static int add2(lua_State* L);
static int sub2(lua_State* L);

static const struct luaL_Reg mylib[] =
{
         {"led_on",lua_led_on},
         {"led_off",lua_led_off},
         {"delay",lua_delay},
         {NULL,NULL}
};



/*******************************************************************************
 * Code
 ******************************************************************************/
static void Delay(int num)
{
  int i, j;
  
  for(i = 0; i < num; i++)
  {
      for(j = 0; j < 3000; j++)
      {
          ;
      }
  }
}

static int lua_led_on(lua_State* L)
{
    PRINTF("LED ON\n");
    return 1;
}

static int lua_led_off(lua_State* L)
{
    PRINTF("LED OFF\n");
    return 1;
}

static int lua_delay(lua_State* L)
{
    int num;

    num= lua_tointeger(L, 1);
    Delay(num);
    
    return 1;
}

int luaopen_mylib(lua_State *L)
{ 
#if 0
         luaL_setfuncs(L,mylib, 0);
#else
         luaL_newlib(L,mylib);
#endif
         return 1;
}

//待Lua调用的C注册函数。
static int add2(lua_State* L)
{
    //检查栈中的参数是否合法，1表示Lua调用时的第一个参数(从左到右)，依此类推。
    //如果Lua代码在调用时传递的参数不为number，该函数将报错并终止程序的执行。
    //double op1 = luaL_checknumber(L,1);
    //double op2 = luaL_checknumber(L,2);
    //将函数的结果压入栈中。如果有多个返回值，可以在这里多次压入栈中。
    //lua_pushnumber(L,op1 + op2);
    
    PRINTF("LED ON\n");
    
    //返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
    return 1;
}

//另一个待Lua调用的C注册函数。
static int sub2(lua_State* L)
{
    double op1 = luaL_checknumber(L,1);
    double op2 = luaL_checknumber(L,2);
    lua_pushnumber(L,op1 - op2);
    return 1;
}

/*!
 * @brief Main function
 */
int main(void)
{   
    lua_State *L;
    int ret = 0;
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
    PRINTF("---------START-----------\n\r");
    L   = luaL_newstate(); 

    luaL_openlibs(L);  
    //luaopen_base(L);  
    lua_register(L,"led_on",lua_led_on);
    lua_register(L,"led_off",lua_led_off);
    lua_register(L,"delay",lua_delay);
    lua_register(L, "add2", add2);
    lua_register(L, "sub2", sub2);
    //luaopen_mylib(L);
    //ret = luaL_dostring(L, testfunc); /* 运行Lua脚本 */  
    //ret = luaL_dostring(L, LUA_SCRIPT_GLOBAL); /* 运行Lua脚本 */  
		ret = luaL_dostring(L, lua_test); /* 运行Lua脚本 */  
    if(ret != 0)
    {
        lua_close(L);  
    }
    lua_close(L);  

    while(1);
}

#if defined(__CC_ARM)
time_t time(time_t * time)  
{  
    return 0;  
}  
  
void exit(int status)  
{  
      
}  
  
int system(const char * string)  
{  
    return 0;  
}  
#endif /* __CC_ARM */


