 #if !defined(FC_MACRO_HERADER___HHHH)
#define FC_MACRO_HERADER___HHHH


// 使用下列四个宏，必须要求类必须从root_object继承，或者root_object的子类进行继承
// 使用这些宏的类必须有一个默认构造函数，即有一个没有参数的构造函数，或者参数有缺省值的构造函数

// DECLARE_DYNAMIC(class_name)
// 动态类型识别声明宏，放在class_name的声明体内

// IMPLEMENT_DYNAMIC(class_name, base_class_name)
// 动态类型识别实现宏，放在class_name的实现体内



// DECLARE_DYNCREATE(class_name)
// 动态对象生成声明宏，放在class_name的声明体内

// IMPLEMENT_DYNCREATE(class_name, base_class_name)
// 动态对象生成实现宏，放在class_name的实现体内


// RUNTIME_CLASS(class_name)
// 得到对应类的动态类型对象

// classType* p = dynamic_create<classType>(class_name_string)
// 在堆上动态生成class_name_string类名的类对象，返回该对象的指针
// 该函数是类型转换安全的，因此可以放心使用

// hp上面使用续行符编译不能通过，所以这些宏定义只能放在同一行中

#define RUNTIME_CLASS(class_name)  (&class_name::class##class_name)

#define DECLARE_DYNAMIC(class_name) public: static runtime_class class##class_name;  virtual runtime_class* get_runtime_class() const;

#define DECLARE_DYNCREATE(class_name) DECLARE_DYNAMIC(class_name)  static root_object* create_object();

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, pfnNew)  static char _lpsz##class_name[] = #class_name;  runtime_class class_name::class##class_name =  { _lpsz##class_name, pfnNew, RUNTIME_CLASS(base_class_name) };  static AFX_CLASSINIT _init_##class_name(&class_name::class##class_name);  runtime_class* class_name::get_runtime_class() const { return &class_name::class##class_name; } 


#define IMPLEMENT_DYNAMIC(class_name, base_class_name)   _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0)

        
#define IMPLEMENT_DYNCREATE(class_name, base_class_name)  root_object* class_name::create_object()  { return new class_name; } 	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, class_name::create_object)

	
class root_object;
typedef const char* LPCSTR ;



struct runtime_class
{
// Attributes
   LPCSTR m_lpszClassName;

   root_object* (* m_pfnCreateObject)(); // NULL => abstract class
   root_object* create_object();
   static runtime_class* load(LPCSTR lpszClassName);
   runtime_class* m_pBaseClass;

   // runtime_class objects linked together in simple list
   static runtime_class* pFirstClass; // start of class list
   runtime_class* m_pNextClass;    // linked list of registered classes
};

struct AFX_CLASSINIT
        { AFX_CLASSINIT(runtime_class* pNewClass); };



class root_object
{
public:
  root_object() ;
  virtual ~root_object();

  virtual runtime_class* get_runtime_class() const;
  bool is_kindof(const runtime_class* pClass) const;

public:
  static runtime_class classroot_object;
};


namespace archy {
		
template <class T>
T* dynamic_create(const char* className)
{
	if( className == 0 ) return 0;
	runtime_class* pClassRef;
	root_object* pOb;
	pClassRef = runtime_class::load(className);
	if( pClassRef == 0 ) {
//		printf("runtime_class::Load failed\n");
		return 0;
	}
	
	pOb = pClassRef->create_object();
	
//	return (T*)pOb;


	T* pRet = dynamic_cast<T*>(pOb);
	if( pRet == 0 ) {
//		printf("dynamic_cast failed\n");
		delete pOb;
	}
	return pRet;

	
}

} // namespace arcky

#endif // FC_MACRO_HERADER___HHHH


