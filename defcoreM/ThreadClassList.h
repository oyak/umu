#ifndef ThreadClassListH
#define ThreadClassListH

#include "platforms.h"

#include <vector>

#if defined (DEFCORE_CC_BOR)
    typedef bool (__closure *TickProcPtr)(void); // возвращает: TRUE - если надо продолжать вызывать, FALSE - не надо, цикл окончен
    //#define DEFCORE_THREAD_FUNCTION(type, obj, fn) obj.fn
	typedef void * TickClassPtr;
	#define DEFCORE_THREAD_FUNCTION(type, obj, fn) 0, obj->fn
#else
    template<typename T, bool (T::* Method)()>
    bool Function(void * param)
    {
        return ((T*)param->*Method)();
    };
    typedef bool (*TickProcPtr)(void *);
    typedef void * TickClassPtr;
    #define DEFCORE_THREAD_FUNCTION(type, obj, fn) obj, Function<type, &type::fn>
#endif

// Класс служит для управления потоками
// Позволяет создавать потоки - AddTick, в параметре передаеся
// исполняемая функция. Потоки запускаются на исполнения сразу после создания
// Контролировать окончание работы потока
// Удалять ранее созданные потоки

class cThreadClassList
{
public:
    virtual ~cThreadClassList() {};
    virtual int AddTick(TickClassPtr object, TickProcPtr function) = 0; // Создает и запускает поток, исполняющий функцию TickProcPtr, Возвращает номер потока
	virtual bool DelTick(int Index) = 0; // Удаляет ранее созданный поток
	virtual bool Finished(int Index) = 0; // Указывает что поток с номером Index закончл работу
    virtual bool Resume(int Index) = 0;
};

#endif
