import sys, os, re
from textwrap import indent, dedent
from math import ceil




def roundUp(x : int, b : int) -> int :
    return b * ceil(x / b)



def translateDataType(glslType : str) -> str :
    return(
        re.sub( r'^vec([234])', r'f32v\g<1>',
        re.sub(r'^bvec([234])',   r'bv\g<1>',
        re.sub(r'^ivec([234])', r'i32v\g<1>',
        re.sub(r'^uvec([234])', r'u32v\g<1>',
        re.sub(r'^dvec([234])', r'f64v\g<1>',
        re.sub(   r'^int',  r'i32', #TODO improve performance
        re.sub(  r'^uint',  r'u32', #TODO improve performance
        re.sub(  r'^bool', r'bool', #TODO improve performance
        re.sub( r'^float',  r'f32', #TODO improve performance
        re.sub(r'^double',  r'f64', #TODO improve performance
        glslType))))))))))
    )




def getTypeSize(type_ : str) -> int :
    return(
        2 * 4 if re.search(r'^[biu]?vec2',           type_) != None else
        2 * 8 if re.search(r'^dvec2',                type_) != None else
        4 * 4 if re.search(r'^[biu]?vec[34]',        type_) != None else
        4 * 8 if re.search(r'^dvec[34]',             type_) != None else
        4     if re.search(r'^((u?int)|float|bool)', type_) != None else
        8     if re.search(r'^double',               type_) != None else
        99999999 #If something bad happens, make it obvious
    )








def createFuncs(members:str, iext:bool) :
    m : str = members.expandtabs(4).strip()
    ret      : str             = ''
    ext      : tuple(str, str) = None
    iext_    : bool            = False
    maxAlign : int             = 0


    offset : int = 0
    while len(m) > 0:
        r = re.search(r'^( |\n){1,}', m)                            #Ignore whitespace
        if r != None:                                                   #
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        r = re.search(r'(^\/\*(.|\n)*?\*\/)|(^\/\/.*?\n)', m)       #Skip comments
        if r != None:                                                   #
            ret += '\n' + r.group(0).strip()                            #Concatenate to return value
            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        if iext_: break                                             #Break if the binding is external and the trailing comment has been written
        r = re.search(                                              #Search for members
            r'^()*?'                                                    # -                       # -     #Anchor to beginning
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 2  3  4  5  6  7  8     # 2     #Skip any comment or whitespace
            r'(([biuv]?vec[234])|(double|float|bool|(u?int)))'          # 9  10 11 12             # 9     #Get type name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 13 14 15 16 17 18 19    # 13    #Skip any comment or whitespace
            r'([a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                             # 20                      # 20    #Get variable name
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 21 22 23 24 25 26 27    # 21    #Skip any comment or whitespace
            r'\[?'                                                      # -                       # -     #Check opening array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 28 29 30 31 32 33 34    # 28    #Skip any comment or whitespace
            r'(\])?'                                                    # 35                      # 35    #Check closing array bracket
            r'((( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*)'             # 36 37 38 39 40 41 42    # 36    #Skip any comment or whitespace
            r';'                                                        # -                       # -     #Anchor to instruction end
            r'((( )|(\/\/.*?(\n|$)))*)',                                # 43 44 45 46 47          # 43    #Skip eventual comments after member declaration
        m)                                                              #
        if r != None:                                               #If a member was found
            _type = r.group(9); _name = r.group(20); _iarr = r.group(35)#(Programmer friendly names)
            _coms = { r.group(2), r.group(13), r.group(21), r.group(28), r.group(36), r.group(43) }
            ttype:str = translateDataType(_type)                        #Translate its type
            align:int = getTypeSize(_type)                              #Get member alignment
            maxAlign = max(maxAlign, align)                             #Recalculate maximum alignment


            for _comm in _coms:                                         #Write comments
                if _comm != None and len(_comm.strip()) > 0: ret += '\n' + dedent(_comm.rstrip())

            if not iext:                                                #If the binding is not external
                ret += '\nalwaysInline ' + ttype + '& '                     #Write translated type
                offset = roundUp(offset, align)                             #Fix element offset and    #Create getter from variable name
                ret += _name + '() { return *(' + ttype + '*)' + ('(ShaderElm_b::data + ' + str(offset) + ')' if offset != 0 else 'ShaderElm_b::data') + '; }'
                offset += align                                             #Calculate raw offset of the next element
            else:                                                       #If the binding is external
                iext_ = True                                                #Set external binding variable
                ext = (ttype, _name)                                        #Save binding type and name. They will be used when writing create()

            m = m[len(r.group(0)):]                                     #Pop parsed string from source string
            continue                                                    #Keep parsing


        #umu umu
        else:                                                       #Blindly copy anything else
            m = m[1:]                                                   #


    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) + 64 }) #Structure size must be a multiple of 16   //BUG THE NORMAL SIZE MAKES THE ENGINE CRASH
    return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 256)) }) #Structure size must be a multiple of 16      //BUG THE NORMAL SIZE MAKES THE ENGINE CRASH
    #!                                                                              ^ NVIDIA has a huge alignment of 256 bytes.
    #FIXME                                                                          ^ USE DIFFERENT ALIGNMENT FOR STORAGE BUFFERS
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) }) #Structure size must be a multiple of 16      //BUG THE NORMAL SIZE MAKES THE ENGINE CRASH








def translateStructDecl(name:str, iext:bool, type:str, binding:int, members:str, space:bool) :
    t = createFuncs(members, iext)
    return dict({
        'decl' : (('\n\n' if space else '') +                                   #Fix spacing
            '\nstruct ' + name + '_t : public ShaderElm_b<' + ('Storage' if type == 'buffer' else 'Uniform') + '> {' +
            indent(                                                             # ^ Struct declaration {
                '\n' + name + '_t() {' + (                                          #Constructor {
                    ('\n\tShaderElm_b::vdata.realloc(' + str(t['size']) + ');') +       #Allocate gpu data
                    ('\n\tShaderElm_b::data.realloc('  + str(t['size']) + ');')         #Allocate local data copy
                    if not iext else ''                                                 #But only if the binding is not ext
                    ) +                                                                 #
                    '\n\tShaderElm_b::bind = ' + str(binding) + ';'                     #Set binding index
                '\n}' +                                                             # }
                t['func'],                                                          #Member access functions(){ ... }
            '\t') +                                                                 #
            '\n} ' + name + ';'                                                 # } MemberDeclaration;
        ),

        'ext': t['ext']                                                         #Forward external binding type and name
    })








pathr  = sys.argv[1]

if not os.path.exists(pathr): print("File does not exist")
elif not pathr.split('.')[-1] == 'comp': print('File is not a vulkan shader')
else:
    spath  = re.search(r'^(.*/).*$', pathr)
    spath = spath.group(1) if spath != None else './'

    shname = re.search(r'^(.*/)?(.*)\..*$', pathr)
    if shname != None: shname = shname.group(2)




with open(spath + shname + '.comp', 'r') as fr, open(spath + shname + '.hpp', 'w') as fh, open(spath + shname + '.cpp', 'w') as fc:
    fname = re.sub(r'^([0-9].*)$', r'_\g<1>', re.sub(r'[^a-zA-Z0-9_]', '_', shname))
    fh.write(                                           #Write to file
        '\n//####################################################################################'
        '\n// This file was generated automatically. Changes could be overwritten without notice'
        '\n//####################################################################################\n'
        '\n#pragma once'                                                    #Include guard
        '\n#include "Lynx/Core/Render/Shaders/Shader_t.hpp"\n\n\n'     #Base Shader struct
        '\nnamespace lnx::shd{'                                             #Write namespace and struct declaration
        '\n\tstruct ' + fname + ' : public Shader_b {'
        '\n\t\tstatic Shader_b::Layout layout;'
        '\n\t\tstatic uint32 pipelineIndex;'
    )
    fc.write(                                           #Write to file
        '\n//####################################################################################'
        '\n// This file was generated automatically. Changes could be overwritten without notice'
        '\n//####################################################################################\n'
        '\n#include "' + re.sub(r'^.*?\/?Lynx\/(Lynx\/.*$)', r'\g<1>', spath + shname) + '.hpp"'
        '\n#include "Lynx/Core/AutoInit.hpp"'                       #Auto init
        '\n#include "Lynx/Core/Render/Window/Window.hpp"'              #Window struct
        '\n#include "Lynx/Core/Render/Shaders/Shader.hpp"'
        '\n#define LNX_H_' + shname.upper() + '\n\n\n'                      #Auto init define
        '\nnamespace lnx::shd{'                                             #Write namespace declaration
    )

    shader = re.findall(                                #Search for binding declarations
        r'(( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*'       # 0 1 2 3 4 5       # -      #Skip comments and whitespace
        r'(ext)?'                                           # 6                 # 6      #Skip comments and whitespace
        r'(( |\n)|((\/\*(.|\n)*?\*\/)|(\/\/.*?\n)))*'       # 7 8 9 10 11 12    # -      #Skip comments and whitespace
        r'layout.*(\(.*binding[\t ]*=[\t ]'                 # 13                # -      #Find layout declaration
        r'([0-9][0-9]?)\))'                                 # 14                # 14     #Get layout binding
        r'[\t ]*(buffer|uniform) (.*)'                      # 15 16             # 15 16  #Get layout type and name
        r'\{(([^\}]|\n)*)\}',                               # 17 18             # 17     #Get layout members
        fr.read())                                          #
    if shader != None:                                  #If there is at leat one binding
        exts:list = []                                      #External bindings data
        elms:list = []
        storageNum = 0; uniformNum = 0
        for layout in shader:                               #For each layout
            _iext = layout[6] != None and len(layout[6]) > 0    #Check if it's external        #BUG CHECK LENGTH IN MEMBER PERSING TOO
            _name : str = layout[16]; _type = layout[15]; _bind = layout[14]
            decl = translateStructDecl(                         #Translate declaration
                _name, _iext, _type, _bind,                         #
                members = layout[17].strip(),                       #Raw members data
                space   = layout != 0                               #Code spacing
            )                                                       #
            fh.write(indent(decl['decl'], '\t\t'))              #Write members to file
            if _iext:                                           #If it's external, save its data
                exts.insert(len(exts), (decl['ext'][0], decl['ext'][1], _name))
            elms.insert(len(elms), { 'type' : _type, 'name' : _name, 'bind' : _bind})

            if _type == 'uniform': uniformNum += 1
            else: storageNum += 1

        fh.write(indent(                                    #Write shader's create functions
            #FIXME CHECK IF EXTERNS NAMES CONFLICT WITH HARD CODED FUNCTION PARAMETERS NAMES
            '\n\n\nvoid create(' + ', '.join(('vram::ptr<' + ext[0] + ', VRam, Storage> p' + ext[1][0].upper() + ext[1][1:]) for ext in exts) + ', const u32v3 vGroupCount, Window& pWindow);'
            '\nvoid createDescriptorSets();'
            '\nvoid createCommandBuffers(const u32v3 vGroupCount, Window& pWindow);'
            '\nvoid updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow);'
            '\nvoid destroy();',
        '\t\t'))


        fc.write(indent(
            '\n\n\nvoid ' + fname + '::create(' + ', '.join(('vram::ptr<' + ext[0] + ', VRam, Storage> p' + ext[1][0].upper() + ext[1][1:]) for ext in exts) + ', const u32v3 vGroupCount, Window& pWindow){' +
                '\n\t''pWindow.addObject_m.lock();' +
                    (''.join(('\n\t\t' + ext[2] + '.vdata = (vram::ptr<char, VRam, Storage>)p' + ext[1][0].upper() + ext[1][1:] + ';') for ext in exts)) +
                    '\n'
                    '\n\t\t''createDescriptorSets();'
                    '\n\t\t''createCommandBuffers(vGroupCount, pWindow);'
                    '\n\t\t''pWindow.swp.shadersCBs.add(commandBuffers[0]);'
                '\n\t''pWindow.addObject_m.unlock();'
            '\n}',
        '\t'))

        fc.write(indent(
            '\n\n\nvoid ' + fname + '::createDescriptorSets(){'
                '\n\t''vk::DescriptorPoolSize sizes[2] = {'
                    '\n\t\tvk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(' + str(storageNum) + '),' + (
                    '\n\t\tvk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(' + str(uniformNum) + ')'
                    if uniformNum > 0 else '\n\t\t{}') +
                '\n\t};'
                '\n\t''auto poolInfo = vk::DescriptorPoolCreateInfo()'
                    '\n\t\t''.setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)'
                    '\n\t\t''.setMaxSets       (1)'
                    '\n\t\t''.setPoolSizeCount (' + str(2 if uniformNum > 0 else 1) + ')'
                    '\n\t\t''.setPPoolSizes    (sizes)'
                '\n\t;'
                '\n\t''switch(core::dvc::graphics.LD.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){'
        	        '\n\t\t''case vk::Result::eErrorFragmentationEXT:  dbg::printError("Fragmentation error");  break;'
        	        '\n\t\t''vkDefaultCases;'
                '\n\t''}'
                '\n\n\n'
                '\n\t''auto allocateSetInfo = vk::DescriptorSetAllocateInfo()'
				    '\n\t\t''.setDescriptorPool     (descriptorPool)'
				    '\n\t\t''.setDescriptorSetCount (1)'
				    '\n\t\t''.setPSetLayouts        (&' + shname + '::layout.descriptorSetLayout)'
                '\n\t'';'
                '\n\t''switch(core::dvc::graphics.LD.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){'
        	        '\n\t\t''case vk::Result::eErrorFragmentedPool:    dbg::printError("Fragmented pool");      break;'
        	        '\n\t\t''case vk::Result::eErrorOutOfPoolMemory:   dbg::printError("Out of pool memory");   break;'
        	        '\n\t\t''vkDefaultCases;'
                '\n\t''}'
                '\n\n\n' +
                '\n\t''vk::WriteDescriptorSet writeSets[' + str(len(elms)) + '];' +
                '\n'.join((
                    '\n\t''auto bufferInfo' + str(i) + ' = vk::DescriptorBufferInfo()'
                        '\n\t\t''.setBuffer (' + b['name'] + '.vdata.cell->csc.buffer)'
                        '\n\t\t''.setOffset (' + b['name'] + '.vdata.cell->localOffset)'
                        '\n\t\t''.setRange  (' + b['name'] + '.vdata.cell->cellSize)'
                    '\n\t;'
                    '\n\t''writeSets[' + str(i) + '] = vk::WriteDescriptorSet()'
                        '\n\t\t''.setDstSet          (descriptorSet)'
                        '\n\t\t''.setDstBinding      ('+ str(b['bind']) + ')'
                        '\n\t\t''.setDescriptorCount (1)'
                        '\n\t\t''.setDescriptorType  (vk::DescriptorType::' + ('eUniformBuffer' if b['type'] == 'uniform' else 'eStorageBuffer') + ')'
                        '\n\t\t''.setPBufferInfo     (&bufferInfo' + str(i) + ')'
                    '\n\t;'
                ) for i, b in enumerate(elms)) +
                '\n\tcore::dvc::graphics.LD.updateDescriptorSets(' + str(len(elms)) + ', writeSets, 0, nullptr);'
            '\n}',
        '\t'))


        fc.write(indent('\n\n\n\n\n\n\n\n'
            '\nvoid ' + fname + '::createCommandBuffers(const u32v3 vGroupCount, Window& pWindow){'
                '\n\t''auto allocateCbInfo = vk::CommandBufferAllocateInfo()'
                    '\n\t\t''.setCommandPool        (pWindow.commandPool)'
                    '\n\t\t''.setLevel              (vk::CommandBufferLevel::ePrimary)'
                    '\n\t\t''.setCommandBufferCount (1)'
                '\n\t'';'
                '\n\t''commandBuffers.resize(1);'
                '\n\t''switch(core::dvc::graphics.LD.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }'
                '\n'
                '\n\t''auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);'
                '\n\t''switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }'
                '\n\t''commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[' + shname + '::pipelineIndex]);'
                '\n\t''commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, ' + shname + '::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);'
                '\n\t''commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);'
                '\n\t''switch(commandBuffers[0].end()){ vkDefaultCases; }'
            '\n}',
        '\t'))


        fc.write(indent('\n\n\n\n\n\n\n\n'
            '\nvoid ' + fname + '::updateCommandBuffers(const u32v3 vGroupCount, Window& pWindow){'
                '\n\t''auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);'
                '\n\t''switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }'
                '\n\t''commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pWindow.pipelines[' + shname + '::pipelineIndex]);'
                '\n\t''commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, ' + shname + '::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);'
                '\n\t''commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);'
                '\n\t''switch(commandBuffers[0].end()){ vkDefaultCases; }'
            '\n}',
        '\t'))


        fc.write(indent('\n\n\n\n\n\n\n\n'
            '\nvoid ' + fname + '::destroy(){'
                '\n\t''//TODO'
            '\n}',
        '\t'))




        fc.write(indent('\n\n\n\n\n\n\n\n'
            '\nShader_b::Layout ' + shname + '::layout;'
            '\nuint32 ' + shname + '::pipelineIndex = core::shaders::pipelineNum++;'
            '\nLnxAutoInit(LNX_H_' + shname.upper() + '){'
                '\n\t''core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);'
                '\n\t''core::shaders::pipelineLayouts[' + shname + '::pipelineIndex] = &' + shname + '::layout;'
                '\n\t{ //Create descriptor set layout'
                    '\n\t\tvk::DescriptorSetLayoutBinding bindingLayouts[' + str(len(elms)) + '];' +
                    '\n'.join((
                        '\n\t\tbindingLayouts[' + str(i) + '] = vk::DescriptorSetLayoutBinding()'
                            '\n\t\t\t.setBinding            (' + str(b['bind']) + ')'
                            '\n\t\t\t.setDescriptorType     (vk::DescriptorType::' + ('eUniformBuffer' if b['type'] == 'uniform' else 'eStorageBuffer') + ')'
                            '\n\t\t\t.setDescriptorCount    (1)'
                            '\n\t\t\t.setStageFlags         (vk::ShaderStageFlagBits::eCompute)'
                            '\n\t\t\t.setPImmutableSamplers (nullptr)'
                        '\n\t\t;'
                    ) for i, b in enumerate(elms)) +
                    '\n'
                    '\n\t\tauto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()'
                        '\n\t\t\t.setBindingCount (' + str(len(elms)) + ')'
                        '\n\t\t\t.setPBindings    (bindingLayouts)'
                    '\n\t\t;'
                    '\n\t\t//Create the descriptor set layout'
                    '\n\t\tswitch(core::dvc::graphics.LD.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &' + shname + '::layout.descriptorSetLayout)){ vkDefaultCases; }'
                '\n\t}'
                '\n'
                '\n'
                '\n'
                '\n'
                '\n\t{ //Create pipeline layout'
                    '\n\t\tuint64 fileLength = 0;'
                    '\n\t\tuint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + "' + shname + '.spv").begin());' #TODO EVALUATE SHADER PATH AT RUNTIME
                    '\n\t\t' + shname + '::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.LD, code, fileLength);'
                    '\n'
                    '\n\t\t' + shname + '::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()'
                        '\n\t\t\t.setStage  (vk::ShaderStageFlagBits::eCompute)'
                        '\n\t\t\t.setModule (' + shname + '::layout.shaderModule)'
                        '\n\t\t\t.setPName  ("main")'
                    '\n\t\t;'
                    '\n'
                    '\n\t\tauto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()'
                        '\n\t\t\t.setSetLayoutCount (1)'
                        '\n\t\t\t.setPSetLayouts    (&' + shname + '::layout.descriptorSetLayout)'
                    '\n\t\t;'
                    '\n\t\tswitch(core::dvc::graphics.LD.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &' + shname + '::layout.pipelineLayout)){ vkDefaultCases; }'
                '\n\t}'
            '\n}',
        '\t'))

        #TODO ADD DESTROY FUNCTION (copy from shaders::destroyShader)
        #FIXME AUTOMATIZE CPP INCLUDES

    else:
        print('No layout found. A shader must define at least one layout')

    fh.write('\n\t};\n}');                              # } //Namespace
    fh.write('//TODO remove local data in external bindings') #TODO
    fc.write('\n}');                              # } //Namespace

#TODO ADD STRUCTURE PARSING AND TRANSLATION
#TODO STRUCTURES HAVE A MINIMUM ALIGNMENT OF 16

#TODO ADD #define PARSING