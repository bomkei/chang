#include "chang.h"

std::vector<Node*> get_return_values(Node* node) {
  if( !node )
    return { };

  switch( node->kind ) {

  }
}

Node* Evaluater::check_integrated(Node* node) {

}

std::tuple<Node*, std::size_t> Evaluater::find_var(std::string_view const& name) {
  for( auto it = scope_list.begin(); it != scope_list.end(); it++ ) {
#if __DEBUG__
    alert;
    fprintf(stderr,"*it = %p\n",*it);
    fprintf(stderr,"(*it)->object.size() = %lu\n",(*it)->objects.size());
#endif

    auto find = (*it)->find_var(name);

    if( find != -1 ) {
      return { *it, find };
    }
  }
  
  return { nullptr, 0 };
}
