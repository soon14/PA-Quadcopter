/*
 * paramsctl.c
 *
 *  Created on: May 24, 2016
 *
 *  四轴飞行控制器  Copyright (C) 2016  李德强
 */

#include <paramsctl.h>

s_params params_cache;

int st = 0;
int r = 0;
pthread_t pthd;
s_engine *e = NULL;
s_params *p = NULL;

int __init(s_engine *engine, s_params *params)
{
	e = engine;
	p = params;
	st = 1;
	r = 1;

	//载入参数
	params_load();

	//启动键盘接收
	pthread_create(&pthd, (const pthread_attr_t*) NULL, (void* (*)(void*)) &params_input, NULL);

	return 0;
}

int __destory(s_engine *e, s_params *p)
{
	r = 0;
	//保存缓存中的参数
	params_save();

	usleep(50 * 1000);

	resetTermios();

	st = 0;

	return 0;
}

int __status()
{
	return st;
}

//保存参数
void params_save()
{
	FILE *fp = fopen(QUAD_PMS_FILE, "wb");
	if (fp == NULL)
	{
		printf("save params error!\n");
		return;
	}
	fwrite(&params_cache, sizeof(char), sizeof(s_params), fp);
	fclose(fp);
}

//载入参数
void params_load()
{
	FILE *fp = fopen(QUAD_PMS_FILE, "rb");
	if (fp == NULL)
	{
		printf("load params error!\n");
		//如果载入失败则重置参数
		params_reset();
		memcpy(&params_cache, p, sizeof(s_params));
		return;
	}
	//载入参数
	fread(p, sizeof(char), sizeof(s_params), fp);
	memcpy(&params_cache, p, sizeof(s_params));
	fclose(fp);
}

//保存参数到缓存
void params_to_cache()
{
	memcpy(&params_cache, p, sizeof(s_params));
}

//从缓存载入参数
void params_from_cache()
{
	memcpy(p, &params_cache, sizeof(s_params));
}

//重置参数
void params_reset()
{
	//XY轴欧拉角PID参数
	p->kp = 4.9;
	p->ki = 4.6;
	p->kd = 3.0;
	//旋转角速度PID参数
	p->kp_v = 11.8;
	p->ki_v = 8.8;
	p->kd_v = 7.3;
	//Z轴欧拉角PID参数
	p->kp_z = 3.8;
	p->ki_z = 1.6;
	p->kd_z = 2.1;
	//Z旋转角速度PID参数
	p->kp_zv = 0;
	p->ki_zv = 0;
	p->kd_zv = 0;
	//XY轴加速度PID参数
	p->kp_a = 0;
	p->ki_a = 0;
	p->kd_a = 0;
	//XY轴中心点校正补偿
	p->cx = 2.2;
	p->cy = -3.8;
	//摇控器3通道起始值
	p->ctl_fb_zero = 1407;
	p->ctl_lr_zero = 1610;
	p->ctl_pw_zero = 1011;
	//显示类型
	p->ctl_type = 10;
}

//键盘接收按键
void params_input()
{
	//按键
	char ch = 0;
	while (r)
	{
		//按键
		ch = getch();
		//7
		if (ch == '7')
		{
			if (p->ctl_type == 0)
			{
				p->kp += CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->kp_v += CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->kp_z += CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->kp_zv += CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->kp_a += CTL_STEP;
			}
			else if (p->ctl_type == 5)
			{
				p->cx += CTL_STEP;
			}
		}
		//8
		else if (ch == '8')
		{
			if (p->ctl_type == 0)
			{
				p->ki += CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->ki_v += CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->ki_z += CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->ki_zv += CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->ki_a += CTL_STEP;
			}
			else if (p->ctl_type == 5)
			{
				p->cy += CTL_STEP;
			}
		}
		//9
		else if (ch == '9')
		{
			if (p->ctl_type == 0)
			{
				p->kd += CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->kd_v += CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->kd_z += CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->kd_zv += CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->kd_a += CTL_STEP;
			}
		}
		//4
		else if (ch == '4')
		{
			if (p->ctl_type == 0)
			{
				p->kp -= CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->kp_v -= CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->kp_z -= CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->kp_zv -= CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->kp_a -= CTL_STEP;
			}
			else if (p->ctl_type == 5)
			{
				p->cx -= CTL_STEP;
			}
		}
		//5
		else if (ch == '5')
		{
			if (p->ctl_type == 0)
			{
				p->ki -= CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->ki_v -= CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->ki_z -= CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->ki_zv -= CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->ki_a -= CTL_STEP;
			}
			else if (p->ctl_type == 5)
			{
				p->cy -= CTL_STEP;
			}
		}
		//6
		else if (ch == '6')
		{
			if (p->ctl_type == 0)
			{
				p->kd -= CTL_STEP;
			}
			else if (p->ctl_type == 1)
			{
				p->kd_v -= CTL_STEP;
			}
			else if (p->ctl_type == 2)
			{
				p->kd_z -= CTL_STEP;
			}
			else if (p->ctl_type == 3)
			{
				p->kd_zv -= CTL_STEP;
			}
			else if (p->ctl_type == 4)
			{
				p->kd_a -= CTL_STEP;
			}
		}
		//-
		else if (ch == '-')
		{
			e->v -= STEP_V;
		}
		//+
		else if (ch == '+')
		{
			if (e->v == 0)
			{
				e->v = PROCTED_SPEED;
			}
			else
			{
				e->v += STEP_V;
			}
		}
		//0
		else if (ch == '0')
		{
			e->v = 0;
		}
		//x轴y轴PID参数
		else if (ch == 'q')
		{
			p->ctl_type = 0;
		}
		//角速度PID_V参数
		else if (ch == 'w')
		{
			p->ctl_type = 1;
		}
		//z轴PID参数
		else if (ch == 'e')
		{
			p->ctl_type = 2;
		}
		//z角速度PID_ZV参数
		else if (ch == 'r')
		{
			p->ctl_type = 3;
		}
		//xy轴加速度PID_A参数
		else if (ch == 't')
		{
			p->ctl_type = 4;
		}
		//陀螺仪校准参数cx、cy、cz
		else if (ch == 'y')
		{
			p->ctl_type = 5;
		}
		//摇控器起始读数
		else if (ch == 'u')
		{
			p->ctl_type = 6;
		}
		//角度
		else if (ch == 'z')
		{
			p->ctl_type = 7;
		}
		//角速度
		else if (ch == 'x')
		{
			p->ctl_type = 8;
		}
		//加速度
		else if (ch == 'c')
		{
			p->ctl_type = 9;
		}
		//电机转速
		else if (ch == 'v')
		{
			p->ctl_type = 10;
		}
		//保存所有参数到缓存
		else if (ch == 'S')
		{
			params_to_cache();
		}
		//读取缓存中的所有参数
		else if (ch == 'L')
		{
			params_from_cache();
		}
		//锁定电机
		else if (ch == 'J')
		{
			e->lock = 1;
		}
		//解锁电机
		else if (ch == 'F')
		{
			e->lock = 0;
		}
		usleep(1);
	}

	st = 0;
}