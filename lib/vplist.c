#include "vplist.h"
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#include "lwqq_export.h"
#else
#define LWQQ_EXPORT
#endif

struct vp_d_table {
   const char* id;
   VP_DISPATCH d;
};

LWQQ_EXPORT
vp_command vp_make_command(VP_DISPATCH dsph, VP_CALLBACK func, ...)
{
   vp_command ret = { 0 };
   ret.dsph = dsph;
   ret.func = func;
   ret.next = NULL;
   va_list args;
   va_start(args, func);
   dsph(NULL, &ret.data, &args);
   va_end(args);
   return ret;
}

LWQQ_EXPORT
vp_list* vp_make_params(VP_DISPATCH dsph, ...)
{
   vp_list* list = malloc(sizeof(*list));
   va_list args;
   va_start(args, dsph);
   dsph(NULL, list, &args);
   va_end(args);
   return list;
}

LWQQ_EXPORT
void vp_do(vp_command cmd, void* retval)
{
   if (cmd.dsph == NULL || cmd.func == NULL)
      return;
   vp_start(cmd.data);
   cmd.dsph(cmd.func, &cmd.data, retval);
   vp_end(cmd.data);

   cmd.dsph = (VP_DISPATCH)NULL;
   cmd.func = (VP_CALLBACK)NULL;
   vp_command* n = cmd.next;
   cmd.next = NULL;
   vp_command* p;
   while (n) {
      vp_start(n->data);
      n->dsph(n->func, &n->data, NULL);
      vp_end(n->data);
      p = n;
      n = n->next;
      free(p);
   }
}

LWQQ_EXPORT
void vp_do_repeat(vp_command cmd, void* retval)
{
   if (cmd.dsph && cmd.func) {
      vp_start(cmd.data);
      cmd.dsph(cmd.func, &cmd.data, retval);
   }

   vp_command* n = cmd.next;
   vp_command* next = NULL;
   while (n) {
      vp_start(n->data);
      next = n->next; // dsph may free n, so backup it (by HenryHu)
      n->dsph(n->func, &n->data, NULL);
      n = next;
   }
}

void vp_cancel(vp_command cmd)
{
   vp_start(cmd.data);
   vp_end(cmd.data);
   cmd.dsph = (VP_DISPATCH)NULL;
   cmd.func = (VP_CALLBACK)NULL;
   vp_command* n = cmd.next;
   cmd.next = NULL;
   vp_command* p;
   while (n) {
      vp_start(n->data);
      vp_end(n->data);
      p = n;
      n = n->next;
      free(p);
   }
}

const vp_command* vp_link(vp_command* head, vp_command* elem)
{
   vp_command* cmd = head;
   while (cmd->next)
      cmd = cmd->next;
   vp_command* item = malloc(sizeof(vp_command));
   memcpy(item, elem, sizeof(vp_command));
   memset(elem, 0, sizeof(vp_command));
   cmd->next = item;
   return item;
}

void vp_unlink(vp_command* head, const vp_command* elem)
{
   if (!elem)
      return;
   vp_command** p_cmd = &head->next;
   while (*p_cmd) {
      if (*p_cmd == elem) {
         *p_cmd = elem->next;
         ((vp_command*)elem)->next = NULL;
         vp_cancel(*elem);
         free((void*)elem);
         return;
      }
      p_cmd = &(*p_cmd)->next;
   }
}

LWQQ_EXPORT
void vp_func_void(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void);
   if (!func) {
      // because malloc(0) make compiler confused
      // let it as 1,it would freed nature
      vp_init(*vp, 1);
      return;
   }
   ((f)func)();
}

LWQQ_EXPORT
void vp_func_p(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*));
      vp_dump(*vp, *va, void*);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   ((f)func)(p1);
}

LWQQ_EXPORT
void vp_func_2p(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, void*);
   if (!func) {
      va_list va;
      va_copy(va, *(va_list*)q);
      vp_init(*vp, sizeof(void*) * 2);
      vp_dump(*vp, va, void*);
      vp_dump(*vp, va, void*);
      va_end(va);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   ((f)func)(p1, p2);
}

LWQQ_EXPORT
void vp_func_2pi(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, void*, int);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 2 + sizeof(int));
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, int);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   int p3 = vp_arg(*vp, int);
   ((f)func)(p1, p2, p3);
}

LWQQ_EXPORT
void vp_func_3p(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, void*, void*);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 3);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   void* p3 = vp_arg(*vp, void*);
   ((f)func)(p1, p2, p3);
}

LWQQ_EXPORT
void vp_func_3pi(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, void*, void*, int);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 3 + sizeof(int));
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, int);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   void* p3 = vp_arg(*vp, void*);
   int p4 = vp_arg(*vp, int);
   ((f)func)(p1, p2, p3, p4);
}

LWQQ_EXPORT
void vp_func_4p(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, void*, void*, void*);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 4);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   void* p3 = vp_arg(*vp, void*);
   void* p4 = vp_arg(*vp, void*);
   ((f)func)(p1, p2, p3, p4);
}

LWQQ_EXPORT
void vp_func_pi(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef void (*f)(void*, int);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) + sizeof(int));
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, int);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   int p2 = vp_arg(*vp, int);
   ((f)func)(p1, p2);
}

LWQQ_EXPORT
void vp_func_p_i(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef int (*f)(void*);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 2);
      vp_dump(*vp, *va, void*);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   int ret = ((f)func)(p1);
   if (q)
      *(int*)q = ret;
}

LWQQ_EXPORT
void vp_func_2p_i(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef int (*f)(void*, void*);
   if (!func) {
      va_list* va = q;
      vp_init(*vp, sizeof(void*) * 2);
      vp_dump(*vp, *va, void*);
      vp_dump(*vp, *va, void*);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   int ret = ((f)func)(p1, p2);
   if (q)
      *(int*)q = ret;
}

LWQQ_EXPORT
void vp_func_3p_i(VP_CALLBACK func, vp_list* vp, void* q)
{
   typedef int (*f)(void*, void*, void*);
   if (!func) {
      va_list va;
      va_copy(va, *(va_list*)q);
      vp_init(*vp, sizeof(void*) * 3);
      vp_dump(*vp, va, void*);
      vp_dump(*vp, va, void*);
      vp_dump(*vp, va, void*);
      va_end(va);
      return;
   }
   void* p1 = vp_arg(*vp, void*);
   void* p2 = vp_arg(*vp, void*);
   void* p3 = vp_arg(*vp, void*);
   int ret = ((f)func)(p1, p2, p3);
   if (q)
      *(int*)q = ret;
}
