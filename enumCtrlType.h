#pragma once

enum enumCtrlType {
	CTRLTYPE_EDIT,
	CTRLTYPE_COOR_EDIT,//无依赖点、原点
	CTRLTYPE_DOUBLE_EDIT,//无依赖值
	CTRLTYPE_INT_EDIT,//无依赖值
	CTRLTYPE_ANGLE_VALUE_EDIT,//无依赖角度 输入度 转换为弧度
	CTRLTYPE_COLOR_HEX,
	CTRLTYPE_LINE_WIDTH,
	CTRLTYPE_COOR_P1_EDIT,//
	CTRLTYPE_COOR_P2_EDIT,
	CTRLTYPE_LEN_EDIT,
	CTRLTYPE_ANGLE_EDIT,
	CTRLTYPE_NULL
};