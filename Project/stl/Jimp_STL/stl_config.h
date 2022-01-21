// Created by Jimp on 2021/7/23.

#ifndef JIMP_STL__STL_CONFIG_H
#define JIMP_STL__STL_CONFIG_H

#define __STL_BEGIN_NAMESPACE namespace Jimp_STL {
#define __STL_END_NAMESPACE }
#define __STL_TEMPLATE_NULL template<>
#define __STL_TRY try
#define __STL_UNWIND(action) catch(...) { action; throw;}

#endif//JIMP_STL__STL_CONFIG_H