/*
 * shared_ptr.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
 *
 * DCL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * DCL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DCL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef _SHARED_PTR_H_
#define _SHARED_PTR_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STD_SHARED_PTR
	#include <memory>
	namespace dbp {
		using std::shared_ptr;
	}
#else
	#ifdef HAVE_STD_TR1_SHARED_PTR
		#include <tr1/memory>
		namespace dbp {
			using std::tr1::shared_ptr;
		}
	#else
		#ifdef HAVE_BOOST_SHARED_PTR
			#include <boost/shared_ptr.hpp>
			namespace dbp {
				using boost::shared_ptr;
			}
		#else
			#ifdef HAVE_BOOST_TR1_SHARED_PTR
				#include <boost/tr1/shared_ptr.hpp>
				namespace dbp {
					using boost::tr1::shared_ptr;
				}
			#endif
		#endif
	#endif
#endif

#endif /*_SHARED_PTR_H_*/

