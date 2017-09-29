/* ----------------------------------------------------------------------------------------------- */
/*  Copyright (c) 2014 - 2017 by Axel Kenzo, axelkenzo@mail.ru                                     */
/*                                                                                                 */
/*  Разрешается повторное распространение и использование как в виде исходного кода, так и         */
/*  в двоичной форме, с изменениями или без, при соблюдении следующих условий:                     */
/*                                                                                                 */
/*   1. При повторном распространении исходного кода должно оставаться указанное выше уведомление  */
/*      об авторском праве, этот список условий и последующий отказ от гарантий.                   */
/*   2. При повторном распространении двоичного кода должна сохраняться указанная выше информация  */
/*      об авторском праве, этот список условий и последующий отказ от гарантий в документации     */
/*      и/или в других материалах, поставляемых при распространении.                               */
/*   3. Ни имя владельца авторских прав, ни имена его соратников не могут быть использованы в      */
/*      качестве рекламы или средства продвижения продуктов, основанных на этом ПО без             */
/*      предварительного письменного разрешения.                                                   */
/*                                                                                                 */
/*  ЭТА ПРОГРАММА ПРЕДОСТАВЛЕНА ВЛАДЕЛЬЦАМИ АВТОРСКИХ ПРАВ И/ИЛИ ДРУГИМИ СТОРОНАМИ "КАК ОНА ЕСТЬ"  */
/*  БЕЗ КАКОГО-ЛИБО ВИДА ГАРАНТИЙ, ВЫРАЖЕННЫХ ЯВНО ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ, НО НЕ             */
/*  ОГРАНИЧИВАЯСЬ ИМИ, ПОДРАЗУМЕВАЕМЫЕ ГАРАНТИИ КОММЕРЧЕСКОЙ ЦЕННОСТИ И ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ */
/*  ЦЕЛИ. НИ В КОЕМ СЛУЧАЕ НИ ОДИН ВЛАДЕЛЕЦ АВТОРСКИХ ПРАВ И НИ ОДНО ДРУГОЕ ЛИЦО, КОТОРОЕ МОЖЕТ    */
/*  ИЗМЕНЯТЬ И/ИЛИ ПОВТОРНО РАСПРОСТРАНЯТЬ ПРОГРАММУ, КАК БЫЛО СКАЗАНО ВЫШЕ, НЕ НЕСЁТ              */
/*  ОТВЕТСТВЕННОСТИ, ВКЛЮЧАЯ ЛЮБЫЕ ОБЩИЕ, СЛУЧАЙНЫЕ, СПЕЦИАЛЬНЫЕ ИЛИ ПОСЛЕДОВАВШИЕ УБЫТКИ,         */
/*  ВСЛЕДСТВИЕ ИСПОЛЬЗОВАНИЯ ИЛИ НЕВОЗМОЖНОСТИ ИСПОЛЬЗОВАНИЯ ПРОГРАММЫ (ВКЛЮЧАЯ, НО НЕ             */
/*  ОГРАНИЧИВАЯСЬ ПОТЕРЕЙ ДАННЫХ, ИЛИ ДАННЫМИ, СТАВШИМИ НЕПРАВИЛЬНЫМИ, ИЛИ ПОТЕРЯМИ ПРИНЕСЕННЫМИ   */
/*  ИЗ-ЗА ВАС ИЛИ ТРЕТЬИХ ЛИЦ, ИЛИ ОТКАЗОМ ПРОГРАММЫ РАБОТАТЬ СОВМЕСТНО С ДРУГИМИ ПРОГРАММАМИ),    */
/*  ДАЖЕ ЕСЛИ ТАКОЙ ВЛАДЕЛЕЦ ИЛИ ДРУГОЕ ЛИЦО БЫЛИ ИЗВЕЩЕНЫ О ВОЗМОЖНОСТИ ТАКИХ УБЫТКОВ.            */
/*                                                                                                 */
/*  ak_hash.h                                                                                      */
/* ----------------------------------------------------------------------------------------------- */
#ifndef __AK_HASH_H__
#define __AK_HASH_H__

/* ----------------------------------------------------------------------------------------------- */
 #include <ak_oid.h>

/* ----------------------------------------------------------------------------------------------- */
/*! Тип данных, определяющий набор перестановок, используемых в ГОСТ 28147-89 и ГОСТ Р 34.11-94.   */
 typedef ak_uint8 kbox[8][16];
 typedef kbox *ak_kbox;

/*! Тип данных, реализующий перестановки на множестве из 8 бит. */
 typedef ak_uint8 sbox[256];
 typedef sbox magma[4];
 typedef sbox *ak_sbox;

/* ----------------------------------------------------------------------------------------------- */
 struct hash;
 typedef struct hash *ak_hash;

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Преобразование перестановок. */
 int ak_kbox_to_sbox( const ak_kbox , sbox , sbox , sbox , sbox );
/*! \brief Преобразование перестановок. */
 int ak_kbox_to_magma( const ak_kbox , magma );

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Тип функции создания дескриптора контекста хеширования. */
 typedef ak_handle ( ak_function_hash )( void );
/*! \brief Функция очистки контекста хеширования. */
 typedef int ( ak_function_hash_clean )( ak_pointer );
/*! \brief Итерационная функция хеширования. */
 typedef int ( ak_function_hash_update )( ak_pointer, const ak_pointer , const size_t );
/*! \brief Функция завершения вычислений и получения конечного результата. */
 typedef ak_buffer ( ak_function_hash_finalize ) ( ak_pointer,
                                                      const ak_pointer , const size_t, ak_pointer );

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Класс, реализующий контекст алгоритима хеширования. */
/*! Класс предоставляет интерфейс для реализации различных бесключевых алгоритмов хеширования
    информации, построенных с использованием итеративных сжимающих отображений. В настоящее время
    с использованием класса \ref hash реализованы следующие отечественные алгоритмы хеширования
     - Стрибог256,
     - Стрибог512,
     - ГОСТ Р 34.11-94 (в настоящее время стандарт выведен из обращения).

    Обращение к методам класса возможно как из функций внутреннего интерфейса библиотеки
    (неэкспортируемые функции) при подключении заголовочного файла \ref ak_hmac.h,
    так и из функций внешнего интерфейса библиотеки (экспортируемые функции) при подключении заголовочного
    файла \ref libakrypt.h

    Общая схема использования объектов класса с применением функций внутреннего интерфейса
    заключается в следующем.
     - Определить объект (переменную типа struct \ref hash) в статической или динамической памяти.
     - Инициализировать контекст объекта класса путем вызова соответствующей производящей функции
       ( ak_hash_create_streebog256(), ak_hash_create_streebog512(), ak_hash_create_gosthash94() ).
     - Выработать хеш-код (код целостности) от заданной области памяти фиксированного размера
       или заданного файла.
     - Уничтожить объект класса путем вызова функции ak_hash_destroy(). Если объект был
       создан в динамической памяти, то для уничтожения необходимо использовать функцию ak_hash_delete().

     Следующий пример демонстрирует использование функций внутреннего интерфейса.

     \code
       struct hash ctx_one;
       ak_hash ctx_two = NULL;
       ak_buffer result_one = NULL;
       ak_uint8 result_two[64];

      // создаем объект в динамической памяти
       ctx_two = malloc( sizeof( struct hash ));

      // инициализируем объекты
       ak_hash_create_streebog256( &ctx_one );
       ak_hash_create_streebog512( ctx_two );

      // вычисляем хеш-код
       result_one = ak_hash_ptr_context( &ctx_one, some_pointer, size, NULL );
       ak_hash_file_context( ctx_two, "filename", result_two );

      // уничтолжаем объекты
       ak_hash_destroy( &ctx_one );
       ctx_two  = ak_hash_delete( ctx_two );
     \endcode


     В случае, когда необходимо определить хеш-код сообщения или данных, длина которых неизвестна
     на момент начала вычислений, можно воспользоваться методами класса \ref compress.
     Данный класс специально разработан для этой цели.
     Полностью работающий пример использования различных методов вычисления
     кодов целостности для бесключевых функций хеширования может быть найден в
     файле \ref example-hash-file.c

     Описание приемов использования класса \ref hash с помощью функций внешнего интерфейса
     (экспортируемых функций) может быть найдено в разделе \ref thash. Также приемы
     использования экспортируемых функций могут быть найдены в следующих тестовых примерах
     - \ref example-hash.c
     - \ref example-hash-oids.c                                                                    */
/* ----------------------------------------------------------------------------------------------- */
 struct hash {
  /*! \brief размер обрабатываемого блока входных данных */
   size_t bsize;
  /*! \brief размер выходного блока (хеш-кода) */
   size_t hsize;
  /*! \brief указатель на внутренние данные контекста */
   ak_pointer data;
  /*! \brief OID алгоритма хеширования */
   ak_oid oid;
  /*! \brief функция очистки контекста */
   ak_function_hash_clean *clean;
  /*! \brief функция обновления состояния контекста */
   ak_function_hash_update *update;
  /*! \brief функция завершения вычислений и получения конечного результата */
   ak_function_hash_finalize *finalize;
 };

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Инициализация контекста функции хеширования. */
 int ak_hash_create( ak_hash , const size_t , const size_t );
/*! \brief Уничтожение контекста функции хеширования. */
 int ak_hash_destroy( ak_hash );
/*! \brief Освобождение памяти из под контекста функции хеширования. */
 ak_pointer ak_hash_delete( ak_pointer );

/*! \brief Инициализация контекста функции бесключевого хеширования ГОСТ Р 34.11-94. */
 int ak_hash_create_gosthash94( ak_hash , ak_handle );
/*! \brief Инициализация контекста функции бесключевого хеширования ГОСТ Р 34.11-2012 (Стрибог256). */
 int ak_hash_create_streebog256( ak_hash );
/*! \brief Инициализация контекста функции бесключевого хеширования ГОСТ Р 34.11-2012 (Стрибог512). */
 int ak_hash_create_streebog512( ak_hash );
/*! \brief Хеширование заданной области памяти. */
 ak_buffer ak_hash_ptr_context( ak_hash , const ak_pointer , const size_t , ak_pointer );
/*! \brief Хеширование заданного файла. */
 ak_buffer ak_hash_file_context( ak_hash , const char*, ak_pointer );

/* ----------------------------------------------------------------------------------------------- */
/*! \brief Проверка корректной работы функции хеширования Стрибог-256 */
 ak_bool ak_hash_test_streebog256( void );
/*! \brief Проверка корректной работы функции хеширования Стрибог-512 */
 ak_bool ak_hash_test_streebog512( void );
/*! \brief Проверка корректной работы функции хеширования ГОСТ Р 34.11-94 */
 ak_bool ak_hash_test_gosthash94( void );

#endif
/* ----------------------------------------------------------------------------------------------- */
/*                                                                                      ak_hash.h  */
/* ----------------------------------------------------------------------------------------------- */
