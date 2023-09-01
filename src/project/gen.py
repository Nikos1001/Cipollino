
### DATA DEFINITION ###

proj_def = {
    'data': [
        'uint32_t fps'
    ]
}

objs_def = {
    'Graphic': {
        'children': ['Layer']
    },

    'Layer': {
        'children': ['Frame'],
        'class_inject': '''
    Frame* getFrameAt(Project* proj, int t);
    Frame* getFrameStartingAt(Project* proj, int t);
''',
        'data': [
            'Name name'
        ]
    },

    'Frame': {
        'children': ['Stroke'],
        'data': [
            'uint32_t begin',
            'uint32_t end'
        ]
    },

    'Stroke': {
        'children': ['Point'],
        'class_inject': '''
#ifndef SERVER
    Mesh mesh;
#endif
    void updateMesh(Project* proj);
''',
        'init_inject': '''
#ifndef SERVER
    mesh.init(1, (int[]){2});
#endif
''',
        'free_inject': '''
#ifndef SERVER
    mesh.free();
#endif
''',
        'update_inject': 'stroke_->updateMesh(this);' 
    },

    'Point': {
        'data': [
            'glm::vec2 pt'
        ]
    },
}

### CODEGEN ###

import os
path_to_script = os.path.realpath(__file__)
os.chdir('/'.join(path_to_script.split('/')[0:-1]))

typename_to_rw_name = {
    'uint32_t': 'U32',
    'glm::vec2': 'Vec2',
    'Name': 'Name'
}

objs = list(objs_def.keys())

obj_idx = {objs[i]: i for i in range(len(objs))}

parent = {obj: None for obj in objs}
for obj in objs:
    if 'children' in objs_def[obj]:
        for child in objs_def[obj]['children']:
            parent[child] = obj

update_injections = {obj: [] for obj in objs}
for obj in objs:
    if 'update_inject' in objs_def[obj]:

        def propagate_update_inject(o):
            update_injections[o].append(obj)
            if 'children' in objs_def[o]:
                for child in objs_def[o]['children']:
                    propagate_update_inject(child)
        
        propagate_update_inject(obj)

action_codes = {
    'add': 0,
    'delete': 1,
    'assign': 2,
    'set': 3
}

def decap_name(name): 
    return name[0].lower() + name[1:]

def cap_name(name):
    return name[0].upper() + name[1:]


if not os.path.isdir('gen'):
    os.mkdir('gen')

def get_add_method_args(obj):
    add_args = ''
    if 'data' in objs_def[obj]:
        for field in objs_def[obj]['data']:
            add_args += field + ', '
    if parent[obj] == None:
        return 'Key key, ' + add_args
    else:
        return 'Key key, Key ' + decap_name(parent[obj]) + ', ' + add_args


with open('gen/project.h', 'w') as f:
    f.write('\n')
    f.write('#ifndef PROJECT_GEN_H\n')
    f.write('#define PROJECT_GEN_H\n')
    f.write('\n')
    f.write('#include "../op.h"\n')
    f.write('#include "../../common/common.h"\n')
    f.write('#include "../../protocol/msg.h"\n')
    f.write('#include "../../render/mesh.h"\n') # stroke
    f.write('\n')
    f.write('class Project;\n')
    for obj in objs:
        f.write('class ' + obj + ';\n')
    f.write('\n')
    for obj in objs:
        f.write('\n')
        f.write('class ' + obj + ' {\n')
        f.write('public:\n')
        f.write('\tvoid init(Key key);\n')
        f.write('\tvoid free();\n')
        f.write('\tKey key;\n')
        if parent[obj] != None:
            f.write('\tKey ' + decap_name(parent[obj]) + ';\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                f.write('\t' + field + ';\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\tArr<Key> ' + decap_name(child) + 's;\n')
        if 'class_inject' in objs_def[obj]:
            f.write(objs_def[obj]['class_inject'] + '\n')
        f.write('};\n')
    f.write('\n')

    f.write('class Project {\n');
    f.write('public:\n')
    f.write('\n')
    f.write('\tvoid init();\n')
    f.write('\tvoid free();\n')
    f.write('\n')
    f.write('\tvoid writeTo(MsgWriter* msg);\n')
    f.write('\tvoid loadFrom(SocketMsg* msg);\n')
    f.write('\tvoid applyUpdate(SocketMsg* msg);\n')
    f.write('\tvoid applyAddUpdate(SocketMsg* msg, Key key);\n')
    f.write('\n')
    for field in proj_def['data']:
        f.write('\t' + field + ';\n')
    f.write('\n')

    for obj in objs:
        f.write('\t' + obj + '* get' + obj + '(Key key);\n')

        f.write('\tvoid add' + obj + '(' + get_add_method_args(obj) + 'ProjectAction* act = NULL);\n')

        f.write('\tvoid delete' + obj + '(Key ' + decap_name(obj) + ', ProjectAction* act = NULL);\n')
        
        if parent[obj] != None:
            f.write('\tvoid assign' + obj + '(Key ' + decap_name(obj) + ', Key ' + decap_name(parent[obj]) + ', ProjectAction* act = NULL);\n') 
        
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\tvoid set' + obj + cap_name(name) + '(Key ' + decap_name(obj) + ', ' + field + ', ProjectAction* act = NULL);\n')

        f.write('\n')

    f.write('\n')
    f.write('\n')
    for obj in objs:
        f.write('\tArr<' + obj + '> ' + decap_name(obj) + 's;\n')
    f.write('};\n')
    f.write('\n')
    f.write('#endif\n')
    f.write('\n')

with open('gen/project.cpp', 'w') as f:

    def insert_update_injections(obj, key_name, indent):
        for inject_parent in update_injections[obj]:
            f.write('\t' * indent + obj + '* ' + decap_name(obj) + '_ = get' + obj + '(' + key_name + ');\n')
            if obj != inject_parent:
                prev_obj = obj
                curr_obj = parent[obj] 
                while prev_obj != inject_parent:
                    f.write('\t' * indent + curr_obj + '* ' + decap_name(curr_obj) + '_ = get' + curr_obj + '(' + decap_name(prev_obj) + '_->' + decap_name(curr_obj) + ');\n')
                    prev_obj = curr_obj
                    curr_obj = parent[curr_obj]
            f.write('\t' * indent + objs_def[inject_parent]['update_inject'] + '\n')

    f.write('\n')
    f.write('#include "project.h"\n')
    f.write('#include "op.h"\n')
    f.write('#include "../../protocol/protocol.h"\n')
    f.write('\n')

    for obj in objs:
        f.write('void ' + obj + '::init(Key key) {\n')
        f.write('\tthis->key = key;\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\t' + decap_name(child) + 's.init();\n')
        if 'init_inject' in objs_def[obj]:
            f.write(objs_def[obj]['init_inject'] + '\n')
        f.write('}\n')
        f.write('\n')
    
        f.write('void ' + obj + '::free() {\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\t' + decap_name(child) + 's.free();\n')
        if 'free_inject' in objs_def[obj]:
            f.write(objs_def[obj]['free_inject'] + '\n')
        f.write('}\n')
        f.write('\n')

    f.write('void Project::loadFrom(SocketMsg* msg) {\n')
    f.write('\tfree();\n')
    f.write('\tinit();\n')
    for field in proj_def['data']:
        type, name = field.split(' ')
        f.write('\t' + name + ' = msg->read' + typename_to_rw_name[type] + '();\n')
    for obj in objs:
        f.write('\tuint32_t n' + obj + ' = msg->readU32();\n')
        f.write('\tfor(int i = 0; i < n' + obj + '; i++) {\n')
        f.write('\t\t' + obj + ' obj;\n')
        f.write('\t\tobj.init(msg->readKey());\n')
        if parent[obj] != None:
            f.write('\t\tobj.' + decap_name(parent[obj]) + ' = msg->readKey();\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\t\tobj.' + name + ' = msg->read' + typename_to_rw_name[type] + '();\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\t\tuint32_t n' + child + ' = msg->readU32();\n')
                f.write('\t\tfor(int j = 0; j < n' + child + '; j++) {\n') 
                f.write('\t\t\tobj.' + decap_name(child) + 's.add(msg->readKey());\n')
                f.write('\t\t}\n')
        f.write('\t\t' + decap_name(obj) + 's.add(obj);\n')
        f.write('\t}\n')
    for obj in objs:
        if 'update_inject' in objs_def[obj]:
            f.write('\tfor(int i = 0; i < ' + decap_name(obj) + 's.cnt(); i++) {\n')
            f.write('\t\t' + obj + '* ' + decap_name(obj) + '_ = &' + decap_name(obj) + 's[i];\n')
            f.write('\t\t' + objs_def[obj]['update_inject'] + '\n')
            f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void Project::writeTo(MsgWriter* msg) {\n')
    for field in proj_def['data']:
        type, name = field.split(' ')
        f.write('\tmsg->write' + typename_to_rw_name[type] + '(' + name + ');\n')
    for obj in objs:
        f.write('\tmsg->writeU32(' + decap_name(obj) + 's.cnt());\n')
        f.write('\tfor(int i = 0; i < ' + decap_name(obj) + 's.cnt(); i++) {\n')
        f.write('\t\t' + obj + '* obj = &' + decap_name(obj) + 's[i];\n')
        f.write('\t\tmsg->writeKey(obj->key);\n')
        if parent[obj] != None:
            f.write('\t\tmsg->writeKey(obj->' + decap_name(parent[obj]) + ');\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\t\tmsg->write' + typename_to_rw_name[type] + '(obj->' + name + ');\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\t\tmsg->writeU32(obj->' + decap_name(child) + 's.cnt());\n')
                f.write('\t\tfor(int j = 0; j < obj->' + decap_name(child) + 's.cnt(); j++) {\n')
                f.write('\t\t\tmsg->writeKey(obj->' + decap_name(child) + 's[j]);\n')
                f.write('\t\t}\n')
        f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void Project::applyUpdate(SocketMsg* msg) {\n')
    f.write('\tuint8_t actionType = msg->readU8();\n')
    f.write('\tif(actionType == ' + str(action_codes['delete']) + ') {\n')
    f.write('\t\tuint32_t objIdx = msg->readU32();\n')
    f.write('\t\tKey key = msg->readKey();\n')
    for obj in objs:
        f.write('\t\tif(objIdx == ' + str(obj_idx[obj]) + ')\n')
        f.write('\t\t\tdelete' + obj + '(key);\n')
    f.write('\t}\n')
    f.write('\tif(actionType == ' + str(action_codes['assign']) + ') {\n')
    f.write('\t\tuint32_t objIdx = msg->readU32();\n')
    f.write('\t\tKey obj = msg->readU32();\n')
    f.write('\t\tKey parent = msg->readU32();\n')
    for obj in objs:
        if parent[obj] != None:
            f.write('\t\tif(objIdx == ' + str(obj_idx[obj]) + ')\n')
            f.write('\t\t\tassign' + obj + '(obj, parent);\n')
    f.write('\t}\n')
    f.write('\tif(actionType == ' + str(action_codes['set']) + ') {\n')
    f.write('\t\tuint32_t objIdx = msg->readU32();\n')
    f.write('\t\tuint32_t fieldIdx = msg->readU32();\n')
    f.write('\t\tKey key = msg->readKey();\n')
    for obj in objs:
        f.write('\t\tif(objIdx == ' + str(obj_idx[obj]) + ') {\n')
        if 'data' in objs_def[obj]:
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('\t\t\tif(fieldIdx == ' + str(i) + ')\n')
                f.write('\t\t\t\tset' + obj + cap_name(name) + '(key, msg->read' + typename_to_rw_name[type] + '());\n')
        f.write('\t\t}\n')
    f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void Project::applyAddUpdate(SocketMsg* msg, Key key) {\n')
    f.write('\tuint32_t objId = msg->readU32();\n')
    for i in range(len(objs)): 
        obj = objs[i]
        f.write('\tif(objId == ' + str(i) + ') {\n')
        if parent[obj] != None:
            f.write('\t\t' + parent[obj] + '* parent = get' + parent[obj] + '(msg->readKey());\n')
            f.write('\t\tif(parent == NULL)\n')
            f.write('\t\t\treturn;\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\t\t' + type + ' ' + name + ' = msg->read' + typename_to_rw_name[type] + '();\n') 
        add_line = '\t\tadd' + obj + '(key'
        if parent[obj] != None:
            add_line += ', parent->key'
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                add_line += ', ' + name
        f.write(add_line + ');\n')
        f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void Project::init() {\n')
    for obj in objs:
        f.write('\t' + decap_name(obj) + 's.init();\n')
    f.write('}\n')
    f.write('\n')
    f.write('void Project::free() {\n')
    for obj in objs:
        f.write('\tfor(int i = 0; i < ' + decap_name(obj) + 's.cnt(); i++) {\n')
        f.write('\t\t' + decap_name(obj) + 's[i].free();\n')
        f.write('\t}\n')
        f.write('\t' + decap_name(obj) + 's.free();\n')
    f.write('}\n')
    f.write('\n')

    for obj in objs:

        f.write('static ' + obj + 'AddData* make' + obj + 'AddData(' + obj + '* obj, ProjectAction* act) {\n')
        f.write('\t' + obj + 'AddData* data = (' + obj + 'AddData*)anim::malloc(sizeof(' + obj + 'AddData));\n') 
        if parent[obj] != None:
            f.write('\tdata->parent = act->getKey(obj->' + decap_name(parent[obj]) + ');\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\tdata->' + name + ' = obj->' + name + ';\n') 
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\tdata->' + decap_name(child) + 's.init();\n')
                f.write('\tfor(int i = 0; i < obj->' + decap_name(child) + 's.cnt(); i++)\n')
                f.write('\t\tdata->' + decap_name(child) + 's.add(act->getKey(obj->' + decap_name(child) + 's[i]));\n')
        f.write('\treturn data;\n')
        f.write('}\n')
        f.write('\n')

        f.write(obj + '* Project::get' + obj + '(Key key) {\n')
        f.write('\tfor(int i = 0; i < ' + decap_name(obj) + 's.cnt(); i++)\n')
        f.write('\t\tif(' + decap_name(obj) + 's[i].key == key)\n')
        f.write('\t\t\treturn &' + decap_name(obj) + 's[i];\n')
        f.write('\treturn NULL;\n')
        f.write('}\n')
        f.write('\n')

        f.write('void Project::add' + obj + '(' + get_add_method_args(obj) + 'ProjectAction* act) {\n')
        if parent[obj] != None:
            f.write('\t' + parent[obj] + '* parent = get' + parent[obj] + '(' + decap_name(parent[obj]) + ');\n')
            f.write('\tif(parent == NULL)\n')
            f.write('\t\treturn;\n')
        f.write('\t' + obj + ' obj' + ';\n')
        f.write('\tobj.init(key);\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\tobj.' + name + ' = ' + name + ';\n')
        if parent[obj] != None:
            f.write('\tobj.' + decap_name(parent[obj]) + ' = parent->key;\n')
            f.write('\tparent->' + decap_name(obj) + 's.add(key);\n')
        f.write('\t' + decap_name(obj) + 's.add(obj);\n')
        insert_update_injections(obj, 'key', 1)
        f.write('\tif(act != NULL) {\n')
        f.write('\t\tProjectOP op;\n')
        f.write('\t\top.key = act->getKey(key);\n')
        f.write('\t\top.type = ' + str(action_codes['add']) + ';\n')
        f.write('\t\top.objType = ' + str(obj_idx[obj]) + ';\n')
        f.write('\t\top.data = (void*)make' + obj + 'AddData(&obj, act);\n')
        f.write('\t\tact->addOP(op);\n')
        f.write('\t}\n')
        f.write('}\n')
        f.write('\n')

        f.write('void Project::delete' + obj + '(Key key, ProjectAction* act) {\n')
        f.write('\t' + obj + '* obj = get' + obj + '(key);\n')
        f.write('\tif(obj == NULL)\n')
        f.write('\t\treturn;\n')
        f.write('\tif(act != NULL) {\n')
        f.write('\t\tProjectOP op;\n')
        f.write('\t\top.key = act->getKey(key);\n')
        f.write('\t\top.type = ' + str(action_codes['delete']) + ';\n')
        f.write('\t\top.objType = ' + str(obj_idx[obj]) + ';\n')
        f.write('\t\top.data = (void*)make' + obj + 'AddData(obj, act);\n')
        f.write('\t\tact->addOP(op);\n')
        f.write('\t}\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\tfor(int i = 0; i < obj->' + decap_name(child) + 's.cnt(); i++)\n')
                f.write('\t\tdelete' + child + '(obj->' + decap_name(child) + 's[i]);\n')
        if parent[obj] != None:
            f.write('\t' + parent[obj] + '* parent = get' + parent[obj] + '(obj->' + decap_name(parent[obj]) + ');\n')
            f.write('\tfor(int i = 0; i < parent->' + decap_name(obj) + 's.cnt(); i++) {\n')
            f.write('\t\tif(parent->' + decap_name(obj) + 's[i] == obj->key) {\n')
            f.write('\t\t\tparent->' + decap_name(obj) + 's.removeAt(i);\n')
            f.write('\t\t\tbreak;\n')
            f.write('\t\t}\n')
            f.write('\t}\n')
            insert_update_injections(parent[obj], 'parent->key', 1)
        f.write('\tfor(int i = 0; i < ' + decap_name(obj) + 's.cnt(); i++) {\n')
        f.write('\t\tif(' + decap_name(obj) + 's[i].key == obj->key) {\n')
        f.write('\t\t\t' + decap_name(obj) + 's[i].free();\n')
        f.write('\t\t\t' + decap_name(obj) + 's.removeAt(i);\n')
        f.write('\t\t}\n')
        f.write('\t}\n')
        f.write('}\n')
        f.write('\n')
        
        if parent[obj] != None:
            f.write('void Project::assign' + obj + '(Key objKey, Key parentKey, ProjectAction* act) {\n')
            f.write('\t' + obj + '* obj = get' + obj + '(objKey);\n')
            f.write('\tif(obj == NULL)\n')
            f.write('\t\treturn;\n')
            f.write('\t' + parent[obj] + '* parent = get' + parent[obj] + '(parentKey);\n')
            f.write('\tif(parent == NULL)\n')
            f.write('\t\treturn;\n')
            f.write('\t' + parent[obj] + '* prevParent = get' + parent[obj] + '(obj->' + decap_name(parent[obj]) + ');\n')
            f.write('\tfor(int i = 0; i < prevParent->' + decap_name(obj) + 's.cnt(); i++) {\n')
            f.write('\t\tif(prevParent->' + decap_name(obj) + 's[i] == objKey) {\n')
            f.write('\t\t\tprevParent->' + decap_name(obj) + 's.removeAt(i);\n')
            f.write('\t\t\tbreak;\n')
            f.write('\t\t}\n')
            f.write('\t}\n')
            f.write('\tdo {\n')
            insert_update_injections(parent[obj], 'prevParent->key', 2)
            f.write('\t} while(0);\n')
            f.write('\tobj->' + decap_name(parent[obj]) + ' = parent->key;\n')
            f.write('\tparent->' + decap_name(obj) + 's.add(obj->key);\n')
            insert_update_injections(parent[obj], 'parent->key', 1)
            f.write('\tif(act != NULL) {\n')
            f.write('\t\tProjectOP op;\n')
            f.write('\t\top.key = act->getKey(objKey);\n')
            f.write('\t\top.type = ' + str(action_codes['assign']) + ';\n')
            f.write('\t\top.objType = ' + str(obj_idx[obj]) + ';\n')
            f.write('\t\top.oldParent = act->getKey(prevParent->key);\n')
            f.write('\t\top.newParent = act->getKey(parent->key);\n')
            f.write('\t\tact->addOP(op);\n')
            f.write('\t}\n')
            f.write('}\n')
            f.write('\n')

        if 'data' in objs_def[obj]:
            for i in range(len(objs_def[obj]['data'])): 
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('void Project::set' + obj + cap_name(name) + '(Key key, ' + field + ', ProjectAction* act) {\n')
                f.write('\t' + obj + '* obj = get' + obj + '(key);\n')
                f.write('\tif(obj == NULL)\n')
                f.write('\t\treturn;\n')
                f.write('\tif(act != NULL) {\n')
                f.write('\t\tProjectOP op;\n')
                f.write('\t\top.key = act->getKey(key);\n')
                f.write('\t\top.type = ' + str(action_codes['set']) + ';\n')
                f.write('\t\top.objType = ' + str(obj_idx[obj]) + ';\n')
                f.write('\t\top.fieldIdx = ' + str(i) + ';\n')
                f.write('\t\top.oldData = anim::malloc(sizeof(' + type + '));\n')
                f.write('\t\t*((' + type + '*)op.oldData) = obj->' + name + ';\n')
                f.write('\t\tProjectOP* prevOp = &act->ops[act->ops.cnt() - 1];\n')
                f.write('\t\tif(act->ops.cnt() > 0 && prevOp->key == act->getKey(key) && prevOp->type == ' + str(action_codes['set']) + ' && prevOp->objType == ' + str(obj_idx[obj]) + ' && prevOp->fieldIdx == ' + str(i) + ') {\n')
                f.write('\t\t\t*((' + type +  '*)op.oldData) = *((' + type + '*)prevOp->oldData);\n')
                f.write('\t\t\tprevOp->free();\n')
                f.write('\t\t\tact->ops.pop();\n')
                f.write('\t\t}\n')
                f.write('\t\top.newData = anim::malloc(sizeof(' + type + '));\n')
                f.write('\t\t*((' + type + '*)op.newData) = ' + name + ';\n')
                f.write('\t\tact->addOP(op);\n')
                f.write('\t}\n')
                f.write('\tobj->' + name + ' = ' + name + ';\n')
                insert_update_injections(obj, 'key', 1)
                f.write('}\n')
                f.write('\n')

with open('gen/op.h', 'w') as f:
    f.write('\n')
    f.write('#ifndef OP_GEN_H\n')
    f.write('#define OP_GEN_H\n')
    for obj in objs:
        f.write('\n')
        f.write('struct ' + obj + 'AddData {\n')
        if parent[obj] != None:
            f.write('\tint parent;\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                f.write('\t' + field + ';\n')
        if 'children' in objs_def[obj]:
            for child in objs_def[obj]['children']:
                f.write('\tArr<int> ' + decap_name(child) + 's;\n')
        f.write('};\n')
    f.write('\n')
    f.write('#endif\n')
    f.write('\n')

with open('gen/op.cpp', 'w') as f:
    f.write('\n')
    f.write('#include "../op.h"\n')
    f.write('#include "op.h"\n')
    f.write('#include "../project.h"\n')
    f.write('#include "../../protocol/protocol.h"\n')
    f.write('\n')
    f.write('void ProjectOP::free() {\n')
    f.write('\tif(type == ' + str(action_codes['add']) + ' || type == ' + str(action_codes['delete']) + ') {\n')
    for obj in objs:
        f.write('\t\tif(objType == ' + str(obj_idx[obj]) + ')\n')
        f.write('\t\t\tanim::free(data, sizeof(' + obj + 'AddData));\n')
    f.write('\t}\n')
    f.write('\tif(type == ' + str(action_codes['set']) + ') {\n')
    for obj in objs:
        if 'data' in objs_def[obj]:
            f.write('\t\tif(objType == ' + str(obj_idx[obj]) + ') {\n')
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ') 
                f.write('\t\t\tif(fieldIdx == ' + str(i) + ') {\n')
                f.write('\t\t\t\tanim::free(oldData, sizeof(' + type + '));\n')
                f.write('\t\t\t\tanim::free(newData, sizeof(' + type + '));\n')
                f.write('\t\t\t}\n')
            f.write('\t\t}\n')
    f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void ProjectOP::writeFwd(MsgWriter* msg, ProjectAction* act) {\n')
    f.write('\tif(type == ' + str(action_codes['add']) + ') {\n')
    f.write('\t\tmsg->writeU8(MessageType::ADD_UPDATE);\n')
    f.write('\t\tmsg->writeKey(act->keys[key]);\n')
    f.write('\t\tmsg->writeU32(objType);\n')
    for obj in objs:
        f.write('\t\tif(objType == ' + str(obj_idx[obj]) + ') {\n')
        f.write('\t\t\t' + obj + 'AddData* addData = (' + obj + 'AddData*)data;\n')
        if parent[obj] != None:
            f.write('\t\t\tmsg->writeKey(act->keys[addData->parent]);\n')
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                f.write('\t\t\tmsg->write' + typename_to_rw_name[type] + '(addData->' + name + ');\n')
        f.write('\t\t}\n')
    f.write('\t}\n')
    f.write('\tif(type == ' + str(action_codes['set']) + ') {\n')
    f.write('\t\tmsg->writeU8(MessageType::UPDATE);\n')
    f.write('\t\tmsg->writeU8(' + str(action_codes['set']) + ');\n')
    f.write('\t\tmsg->writeU32(objType);\n')
    f.write('\t\tmsg->writeU32(fieldIdx);\n')
    f.write('\t\tmsg->writeKey(act->keys[key]);\n')
    for obj in objs:
        if 'data' in objs_def[obj]:
            f.write('\t\tif(objType == ' + str(obj_idx[obj]) + ') {\n')
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('\t\t\tif(fieldIdx == ' + str(i) + ')\n')
                f.write('\t\t\t\tmsg->write' + typename_to_rw_name[type] + '(*((' + type + '*)newData));\n')
            f.write('\t\t}\n')
    f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void ProjectOP::writeBwd(MsgWriter* msg, ProjectAction* act) {\n')
    f.write('\tif(type == ' + str(action_codes['add']) + ') {\n')
    f.write('\t\tmsg->writeU8(MessageType::UPDATE);\n')
    f.write('\t\tmsg->writeU8(' + str(action_codes['delete']) + ');\n')
    f.write('\t\tmsg->writeU32(objType);\n')
    f.write('\t\tmsg->writeKey(act->keys[key]);\n')
    f.write('\t}\n')
    f.write('\tif(type == ' + str(action_codes['set']) + ') {\n')
    f.write('\t\tmsg->writeU8(MessageType::UPDATE);\n')
    f.write('\t\tmsg->writeU8(' + str(action_codes['set']) + ');\n')
    f.write('\t\tmsg->writeU32(objType);\n')
    f.write('\t\tmsg->writeU32(fieldIdx);\n')
    f.write('\t\tmsg->writeKey(act->keys[key]);\n')
    for obj in objs:
        if 'data' in objs_def[obj]:
            f.write('\t\tif(objType == ' + str(obj_idx[obj]) + ') {\n')
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('\t\t\tif(fieldIdx == ' + str(i) + ')\n')
                f.write('\t\t\t\tmsg->write' + typename_to_rw_name[type] + '(*((' + type + '*)oldData));\n')
            f.write('\t\t}\n')
    f.write('\t}\n')
    f.write('}\n')

if not os.path.isdir('../client/gen'):
    os.mkdir('../client/gen')

with open('../client/gen/action.cpp', 'w') as f:
    f.write('\n') 
    f.write('#include "../action.h"\n')
    f.write('#include "../editor.h"\n')
    f.write('#include "../../project/gen/op.h"\n')
    f.write('\n')
    f.write('void EditorAction::undo() {\n')
    f.write('\tfor(int i = ops.cnt() - 1; i >= 0; i--) {\n')
    f.write('\t\tif(ops[i].type == ' + str(action_codes['add']) + ') {\n')
    for obj in objs:
        f.write('\t\t\tif(ops[i].objType == ' + str(obj_idx[obj]) + ') {\n')
        f.write('\t\t\t\teditor->proj.delete' + obj + '(keys[ops[i].key]);\n')
        f.write('\t\t\t}\n')
    f.write('\t\t}\n')
    f.write('\t\tif(ops[i].type == ' + str(action_codes['set']) + ') {\n')
    for obj in objs:
        if 'data' in objs_def[obj]:
            f.write('\t\t\tif(ops[i].objType == ' + str(obj_idx[obj]) + ') {\n')
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('\t\t\t\tif(ops[i].fieldIdx == ' + str(i) + ') {\n')
                f.write('\t\t\t\t\teditor->proj.set' + obj + cap_name(name) + '(keys[ops[i].key], *((' + type + '*)ops[i].oldData));\n')
                f.write('\t\t\t\t}\n')
            f.write('\t\t\t}\n')
    f.write('\t\t}\n')
    f.write('\t\tMsgWriter msg;\n')
    f.write('\t\tmsg.init();\n')
    f.write('\t\tops[i].writeBwd(&msg, this);\n')
    f.write('\t\teditor->sock->send(msg.getData(), msg.getSize());\n')
    f.write('\t\tmsg.free();\n')
    f.write('\t}\n')
    f.write('}\n')
    f.write('\n')

    f.write('void EditorAction::redo() {\n')
    f.write('\tfor(int i = 0; i < ops.cnt(); i++) {\n')
    f.write('\t\tif(ops[i].type == ' + str(action_codes['add']) + ') {\n')
    for obj in objs:
        f.write('\t\t\tif(ops[i].objType == ' + str(obj_idx[obj]) + ') {\n')
        f.write('\t\t\t\t' + obj + 'AddData* data = (' + obj + 'AddData*)ops[i].data;\n')
        f.write('\t\t\t\tkeys[ops[i].key] = editor->keys.nextKey();\n')
        add_line = '\t\t\t\teditor->proj.add' + obj + '(keys[ops[i].key]'
        if parent[obj] != None:
            add_line += ', keys[data->parent]'
        if 'data' in objs_def[obj]:
            for field in objs_def[obj]['data']:
                type, name = field.split(' ')
                add_line += ', data->' + name
        add_line += ');\n'
        f.write(add_line)
        f.write('\t\t\t}\n')
    f.write('\t\t}\n')
    f.write('\t\tif(ops[i].type == ' + str(action_codes['set']) + ') {\n')
    for obj in objs:
        if 'data' in objs_def[obj]:
            f.write('\t\t\tif(ops[i].objType == ' + str(obj_idx[obj]) + ') {\n')
            for i in range(len(objs_def[obj]['data'])):
                field = objs_def[obj]['data'][i]
                type, name = field.split(' ')
                f.write('\t\t\t\tif(ops[i].fieldIdx == ' + str(i) + ') {\n')
                f.write('\t\t\t\t\teditor->proj.set' + obj + cap_name(name) + '(keys[ops[i].key], *((' + type + '*)ops[i].newData));\n')
                f.write('\t\t\t\t}\n')
            f.write('\t\t\t}\n')
    f.write('\t\t}\n')
    f.write('\t\tMsgWriter msg;\n')
    f.write('\t\tmsg.init();\n')
    f.write('\t\tops[i].writeFwd(&msg, this);\n')
    f.write('\t\teditor->sock->send(msg.getData(), msg.getSize());\n')
    f.write('\t\tmsg.free();\n')
    f.write('\t}\n')
    f.write('}\n')
