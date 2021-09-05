import os, re, subprocess
from textwrap import indent
from math import ceil
#!Shaders are validated in lynxg++






def roundUp(x : int, b : int) -> int :
    return b * ceil(x / b)






#Parses a single layout element
#iExt: True if thhe binding is an external buffer
def parseElms(elms:str, iExt:bool) :
    elms = elms.expandtabs(4)
    ret      : str             = ''
    ext      : dict            = None
    maxAlign : int             = 0


    typeName = {
        'bvec2': 'bv2',    'ivec2': 'i32v2',    'uvec2': 'u32v2',    'vec2': 'f32v2',    'dvec2': 'f64v2',
        'bvec3': 'bv3',    'ivec3': 'i32v3',    'uvec3': 'u32v3',    'vec3': 'f32v3',    'dvec3': 'f64v3',
        'bvec4': 'bv4',    'ivec4': 'i32v4',    'uvec4': 'u32v4',    'vec4': 'f32v4',    'dvec4': 'f64v4',
         'bool': 'bool',     'int': 'i32',       'uint': 'u32',     'float': 'f32',     'double': 'f64'
    }
    typeSize = {
        'bvec2': 4 * 2,    'ivec2': 4 * 2,      'uvec2': 4 * 2,      'vec2': 4 * 2,      'dvec2': 8 * 2,
        'bvec3': 4 * 4,    'ivec3': 4 * 4,      'uvec3': 4 * 4,      'vec3': 4 * 4,      'dvec3': 8 * 4,
        'bvec4': 4 * 4,    'ivec4': 4 * 4,      'uvec4': 4 * 4,      'vec4': 4 * 4,      'dvec4': 8 * 4,
         'bool': 4,          'int': 4,           'uint': 4,         'float': 4,         'double': 8
    }


    elmsr = re.findall(                                                 #Search for members
        r'(?P<type>([biud]?vec[234])|(double|float|bool|(u?int))) '     #Get type name
        r'(?P<name>[a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                         #Get variable name
        r'(?P<iArr>\[(?P<aLen>.+?)\])?'                                 #Check if it's an array and get its length
        r';',                                                           #Anchor to instruction end
    elms)


    offset : int = 0
    if elmsr is not None:
        for elm in elmsr:
            ttype:str = typeName[elmsr['type']]                       #Translate type
            align:int = typeSize[elmsr['type']]                       #Get alignment
            maxAlign = max(maxAlign, align)                             #Recalculate maximum alignment #TODO check if this actually works

            if not iExt:                                                #If the binding is not external
                ret += '\nalwaysInline ' + ttype + '& '                     #Write translated type
                offset = roundUp(offset, align)                             #Fix element offset
                ret += (elm['name'] + '() {'                            #Create getter from variable name
                    'return *(' + ttype + '*)' + ('(ShaderElm_b::data + ' + str(offset) + ')' if offset != 0 else 'ShaderElm_b::data') + ';'
                '}')                                                        #
                offset += align                                             #Calculate raw offset of the next element #TODO check if this actually works
            else:                                                       #If the binding is external
                ext = dict{'type' : ttype, 'varname': elm['name']}    #Save binding type and name. They will be used when writing create()
                break                                                       #Exit loop #FIXME parse other elements too




    # while len(elms) > 0:
    #     if iext_: break                                             #Break if the binding is external and the trailing comment has been written
    #     if r != None:                                               #If a member was found
    #         ttype:str = typeC[r.group('type')]                          #Translate its type
    #         align:int = typeSize[r.group('type')]                       #Get member alignment
    #         maxAlign = max(maxAlign, align)                             #Recalculate maximum alignment

    #         if not iext:                                                #If the binding is not external
    #             ret += '\nalwaysInline ' + ttype + '& '                     #Write translated type
    #             offset = roundUp(offset, align)                             #Fix element offset and    #Create getter from variable name
    #             ret += r.group('name') + '() { return *(' + ttype + '*)' + ('(ShaderElm_b::data + ' + str(offset) + ')' if offset != 0 else 'ShaderElm_b::data') + '; }'
    #             offset += align                                             #Calculate raw offset of the next element
    #         else:                                                       #If the binding is external
    #             iext_ = True                                                #Set external binding variable
    #             ext = dict({'type' : ttype, 'varname': r.group('name')})              #Save binding type and name. They will be used when writing create()
    #         elms = elms[len(r.group(0)):]                                     #Pop parsed string from source string
    #         continue                                                    #Keep parsing
    #     else:                                                       #Blindly copy anything else
    #         elms = elms[1:]                                                   #umu umu



    #Return a dictionary containing the translated members, the external bindings and the padded structure size
    #The size of the structure must be a multiple of 16 #BUG THE NORMAL SIZE MAKES THE ENGINE CRASH (prob buffer overflow?)
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) + 64})    #BUG ok
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16))})         #BUG crash
    return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 256)) })
    #!NVIDIA has a huge alignment of 256 bytes. #TODO use a different alignment based on the GPU, ig
    #FIXME use different alignment for storage buffers








#Translates a single layout
def parseLayout(name:str, iext:bool, type:str, indx:int, elms:str, space:bool) :
    t = parseElms(elms, iext)
    return dict({
        'decl' : (('\n\n' if space else '') +                                   #Fix spacing
            '\nstruct ' + name + '_t : public ShaderElm_b<' + ('eStorage' if type == 'buffer' else 'eUniform') + '> {' +
            indent(                                                             # ^ Struct declaration {
                '\n' + name + '_t() {' + (                                          #Constructor {
                    ('\n\tShaderElm_b::vdata.realloc(' + str(t['size']) + ');') +       #Allocate gpu data
                    ('\n\tShaderElm_b::data.realloc('  + str(t['size']) + ');')         #Allocate local data copy
                    if not iext else ''                                                 #But only if the binding is not ext
                    ) +                                                                 #
                    '\n\tShaderElm_b::bind = ' + str(indx) + ';'                     #Set binding index
                '\n}' +                                                             # }
                t['func'],                                                          #Member access functions(){ ... }
            '\t') +                                                                 #
            '\n} ' + name + ';'                                                 # } MemberDeclaration;
        ),

        'ext': t['ext']                                                         #Forward external binding type and name
    })








#Parses a GLSL compute shader and writes 2 C++ interface files
def parseShader(pathr:str, ptfm:str):
    if not os.path.exists(pathr):
        print("File does not exist")
        return 1
    else:
        spath  = re.search(r'^(.*/).*$', pathr)
        spath = spath.group(1) if spath != None else './'

        shname = re.search(r'^(.*/)?(.*)\..*$', pathr)
        if shname != None: shname = shname.group(2)




    with open(spath + shname + '.gsi.hpp', 'w') as fh, open(spath + shname + '.gsi.cpp', 'w') as fc:
        fname = re.sub(r'^([0-9].*)$', r'_\g<1>', re.sub(r'[^a-zA-Z0-9_]', '_', shname))

        #Write header
        fh.write(
            '\n//####################################################################################'
            '\n// This file was generated by Lynx/shaders/GlslToCpp.py'
            '\n// Changes could be overwritten without notice'
            '\n//####################################################################################\n'
            '\n#pragma once'                                            #Include guard
            '\n#include "Lynx/Core/Render/Shaders/Shader_t.hpp"\n\n\n'  #Base Shader struct
            '\nnamespace lnx::shd{'                                     #Write namespace and struct declaration
            '\n\tstruct ' + fname + ' : public Shader_b {'
            '\n\t\tstatic Shader_b::Layout layout;'
            '\n\t\tstatic uint32 pipelineIndex;'
        )

        #Write cpp
        fc.write(
            '\n//####################################################################################'
            '\n// This file was generated by Lynx/shaders/GlslToCpp.py'
            '\n// Changes could be overwritten without notice'
            '\n//####################################################################################\n'
            '\n#include "' + re.sub(r'^.*?\/?Lynx\/(Lynx\/.*$)', r'\g<1>', spath + shname) + '.gsi.hpp"' #FIXME
            '\n#include "Lynx/Core/AutoInit.hpp"'                       #Auto init
            # '\n#include "Lynx/Core/Render/Render.hpp"'                  #Window struct
            '\n#include "Lynx/Core/Render/Shaders/Shader.hpp"'
            '\n#define LNX_H_' + fname.upper() + '\n\n\n'               #Auto init define
            '\nnamespace lnx::shd{'                                     #Write namespace declaration
        )




        #Expand macros
        code = subprocess.check_output(
            ['glslangValidator', spath + shname + '.comp', '-E'],
            universal_newlines=True
        )


        #Parse out unnecessary whitespace and comments from the shader code
        ncode = (
            re.sub(r'([()\[\]{}+*-\/.!<>=&^|?:%,;]) ', r'\g<1>',    #Remove spaces after  opeartors
            re.sub(r' ([()\[\]{}+*-\/.!<>=&^|?:%,;])', r'\g<1>',    #Remove spaces before opeartors
            re.sub(r'\+ \+', r'-0-',                                #FIXME
            re.sub(r'\- \-', r'-0-',                                #FIXME
            re.sub(r'\\n', r'\n',                                   #Remove newlines
            re.sub(r'\n', r'',                                      #Remove newlines
            re.sub(r'(#.*?)\n', r'\g<1>\\n',                        #Remove newlines
            re.sub(r' +', r' ',                                     #Remove whitespace
            str(code).expandtabs(4),                                #Convert tabs to spaces
        )))))))))


        #Search for layout declarations
        layouts = [m.groupdict() for m in re.finditer(
            r'layout.*?\(.*?binding=(?P<indx>[0-9]+)\)'
            r'(?P<type>buffer|uniform) (?P<iext>ext_)?(?P<name>.*?)\{(?P<elms>.*?)\}',
        ncode)]

        if layouts != None:                                      #If there is at least one binding
            exts:list = []                                          #List of external bindings
            elms:list = []                                          #List of elements
            storageNum = 0; uniformNum = 0                          #Number of storage and uniform bindings in the shader
            for l in layouts:                                       #For each layout
                _iext = l['iext'] != None and len(l['iext']) > 0        #Check if it's external        #BUG CHECK LENGTH IN MEMBER PARSING TOO
                decl = parseLayout(                             #Translate declaration
                    l['name'], _iext, l['type'], l['indx'],                 #
                    elms = l['elms'].strip(),                               #Raw members data
                    space   = l != 0                                        #Code spacing
                )                                                           #
                fh.write(indent(decl['decl'], '\t\t'))                  #Write members to file
                if _iext:                                               #If it's external, save its data
                    exts.insert(len(exts), {
                        'vartype' : decl['ext']['type'],
                        'varname' : decl['ext']['varname'],
                        'bndtype' : ('eStorage' if l['type'] == 'buffer' else 'eUniform'),
                        'bndname' : l['name']
                    })
                elms.insert(len(elms), { 'type' : l['type'], 'name' : l['name'], 'indx' : l['indx']})

                if l['type'] == 'uniform': uniformNum += 1
                else: storageNum += 1


            #Write the shader create functions
            fh.write(indent(
                #FIXME CHECK IF EXTERNS NAMES CONFLICT WITH HARD CODED FUNCTION PARAMETERS NAMES
                '\n\n\nvoid create(' +
                ', '.join((
                    'vram::ptr<' + ext['vartype'] + ', eVRam, ' + ext['bndtype'] + '> p' +
                    ext['varname'][0].upper() + ext['varname'][1:]
                ) for ext in exts) + ', const u32v3 vGroupCount, core::RenderCore& pRenderCore);'
                '\nvoid createDescriptorSets();'
                '\nvoid createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);'
                '\nvoid updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);'
                '\nvoid destroy();',
            '\t\t'))

            fc.write(indent(
                '\n\n\nvoid ' + fname + '::create(' +
                ', '.join((
                        'vram::ptr<' + ext['vartype'] + ', eVRam, ' + ext['bndtype'] + '> p' +
                        ext['varname'][0].upper() + ext['varname'][1:]
                    )for ext in exts) + ', const u32v3 vGroupCount, core::RenderCore& pRenderCore){' +
                    '\n\t''pRenderCore.addObject_m.lock();' +
                        (
                            ''.join(('\n\t\t' + ext['bndname'] + '.vdata = (vram::ptr<char, eVRam, ' + ext['bndtype'] + '>)p' +
                            ext['varname'][0].upper() + ext['varname'][1:] + ';'
                        ) for ext in exts)) +
                        '\n'
                        '\n\t\t''createDescriptorSets();'
                        '\n\t\t''createCommandBuffers(vGroupCount, pRenderCore);'
                        '\n\t\t''pRenderCore.swp.shadersCBs.add(commandBuffers[0]);'
                    '\n\t''pRenderCore.addObject_m.unlock();'
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
                    '\n\t''switch(core::dvc::graphics.ld.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){'
            	        '\n\t\t''case vk::Result::eErrorFragmentationEXT:  dbg::logError("Fragmentation error");  break;'
            	        '\n\t\t''vkDefaultCases;'
                    '\n\t''}'
                    '\n\n\n'
                    '\n\t''auto allocateSetInfo = vk::DescriptorSetAllocateInfo()'
    				    '\n\t\t''.setDescriptorPool     (descriptorPool)'
    				    '\n\t\t''.setDescriptorSetCount (1)'
    				    '\n\t\t''.setPSetLayouts        (&' + fname + '::layout.descriptorSetLayout)'
                    '\n\t'';'
                    '\n\t''switch(core::dvc::graphics.ld.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){'
            	        '\n\t\t''case vk::Result::eErrorFragmentedPool:    dbg::logError("Fragmented pool");      break;'
            	        '\n\t\t''case vk::Result::eErrorOutOfPoolMemory:   dbg::logError("Out of pool memory");   break;'
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
                            '\n\t\t''.setDstBinding      ('+ str(b['indx']) + ')'
                            '\n\t\t''.setDescriptorCount (1)'
                            '\n\t\t''.setDescriptorType  (vk::DescriptorType::' + ('eUniformBuffer' if b['type'] == 'uniform' else 'eStorageBuffer') + ')'
                            '\n\t\t''.setPBufferInfo     (&bufferInfo' + str(i) + ')'
                        '\n\t;'
                    ) for i, b in enumerate(elms)) +
                    '\n\tcore::dvc::graphics.ld.updateDescriptorSets(' + str(len(elms)) + ', writeSets, 0, nullptr);'
                '\n}',
            '\t'))


            fc.write(indent('\n\n\n\n\n\n\n\n'
                '\nvoid ' + fname + '::createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){'
                    '\n\t''auto allocateCbInfo = vk::CommandBufferAllocateInfo()'
                        '\n\t\t''.setCommandPool        (pRenderCore.commandPool)'
                        '\n\t\t''.setLevel              (vk::CommandBufferLevel::ePrimary)'
                        '\n\t\t''.setCommandBufferCount (1)'
                    '\n\t'';'
                    '\n\t''commandBuffers.resize(1);'
                    '\n\t''switch(core::dvc::graphics.ld.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){ vkDefaultCases; }'
                    '\n'
                    '\n\t''auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);'
                    '\n\t''switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }'
                    '\n\t''commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[' + fname + '::pipelineIndex]);'
                    '\n\t''commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, ' + fname + '::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);'
                    '\n\t''commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);'
                    '\n\t''switch(commandBuffers[0].end()){ vkDefaultCases; }'
                    #TODO WRITE ALL COMMAND BUFFERS AT ONCE
                    #TODO or use multiple descriptor sets for multiple objects, but in the same command buffer
                '\n}',
            '\t'))


            fc.write(indent('\n\n\n\n\n\n\n\n'
                '\nvoid ' + fname + '::updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){'
                    '\n\t''auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);'
                    '\n\t''switch(commandBuffers[0].begin(beginInfo)){ vkDefaultCases; }'
                    '\n\t''commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[' + fname + '::pipelineIndex]);'
                    '\n\t''commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, ' + fname + '::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);'
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
                '\nShader_b::Layout ' + fname + '::layout;'
                '\nuint32 ' + fname + '::pipelineIndex = core::shaders::pipelineNum++;'
                '\nLnxAutoInit(LNX_H_' + fname.upper() + '){'
                    '\n\t''core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);'
                    '\n\t''core::shaders::pipelineLayouts[' + fname + '::pipelineIndex] = &' + fname + '::layout;'
                    '\n\t{ //Create descriptor set layout'
                        '\n\t\tvk::DescriptorSetLayoutBinding bindingLayouts[' + str(len(elms)) + '];' +
                        '\n'.join((
                            '\n\t\tbindingLayouts[' + str(i) + '] = vk::DescriptorSetLayoutBinding()'
                                '\n\t\t\t.setBinding            (' + str(b['indx']) + ')'
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
                        '\n\t\tswitch(core::dvc::graphics.ld.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &' + fname + '::layout.descriptorSetLayout)){ vkDefaultCases; }'
                    '\n\t}'
                    '\n'
                    '\n'
                    '\n'
                    '\n'
                    '\n\t{ //Create pipeline layout'
                        '\n\t\tuint64 fileLength = 0;'
                        '\n\t\tuint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + "' + shname + '.spv").begin());' #TODO EVALUATE SHADER PATH AT RUNTIME
                        '\n\t\t' + fname + '::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.ld, code, fileLength);'
                        '\n'
                        '\n\t\t' + fname + '::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()'
                            '\n\t\t\t.setStage  (vk::ShaderStageFlagBits::eCompute)'
                            '\n\t\t\t.setModule (' + fname + '::layout.shaderModule)'
                            '\n\t\t\t.setPName  ("main")'
                        '\n\t\t;'
                        '\n'
                        '\n\t\tauto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()'
                            '\n\t\t\t.setSetLayoutCount (1)'
                            '\n\t\t\t.setPSetLayouts    (&' + fname + '::layout.descriptorSetLayout)'
                        '\n\t\t;'
                        '\n\t\tswitch(core::dvc::graphics.ld.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &' + fname + '::layout.pipelineLayout)){ vkDefaultCases; }'
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

    return 0