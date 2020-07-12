#pragma once


namespace lux {
	template <class cntType, class alloc> class ContainerBase {
	public:
		virtual inline cntType* __vectorcall begin() const = 0;
		virtual inline cntType* __vectorcall end() const = 0;
		virtual inline alloc __vectorcall size() const = 0;
		virtual inline bool __vectorcall empty() const = 0;
	};
}