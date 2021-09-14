import os, re, subprocess
from textwrap import indent
from math import ceil
#! Shaders are validated in lynxg++
#! This script is compiled with lynxg++


#TODO write what external bindings are and how to use them





# Element parsing ---------------------------------------------------------------------------------------------------------------#








def roundUp(x : int, b : int) -> int :
    return b * ceil(x / b)






#Parses a single layout element
#iExt: True if thhe binding is an external buffer
def parseElms(glsl:str, iExt:bool) :
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


    offset : int = 0
    for elmr in re.finditer(                                    # For each binding member
        r'(?P<type>([biud]?vec[234])|(double|float|bool|(u?int))) ' # Get type name
        r'(?P<name>[a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                     # Get member name
        r'(?P<iArr>\[(?P<aLen>.+?)?\])?'                            # Check if it's an array and get its length
        r';',                                                       # Anchor to instruction end
    glsl):                                                      #
        elm = elmr.groupdict()                                      # Get result as dictionary
        ttype:str = typeName[elmr['type']]                          # Translate type
        align:int = typeSize[elmr['type']]                          # Get type alignment
        maxAlign = max(maxAlign, align)                             # Recalculate maximum alignment #TODO check if this actually works

        if not iExt:                                                # If the binding is not external
            ret += f"\nalwaysInline { ttype }& "                        # Write translated type
            offset = roundUp(offset, align)                             # Recalculate element offset
            ret += (elm['name'] + '() { '                               # Create getter from variable name
                f"return *({ ttype }*){ f'(ShaderElm_b::data + { str(offset) })' if offset != 0 else f'ShaderElm_b::data' };"
            ' }')                                                       #
            offset += align                                             # Calculate raw offset of the next element #TODO check if this actually works
        else:                                                       # If the binding is external
            ext = {'type' : ttype, 'varname': elm['name']}              # Save binding type and name. They will be used when writing create()
            break                                                       # Exit loop #FIXME parse other elements too




    # Return a dictionary containing the translated members, the external bindings and the padded structure size
    # The size of the structure must be a multiple of 16 #BUG THE NORMAL SIZE MAKES THE ENGINE CRASH (prob buffer overflow?)
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) + 64})    #BUG ok
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16))})         #BUG crash
    return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 256)) })
    # !NVIDIA has a huge alignment of 256 bytes. #TODO use a different alignment based on the GPU, ig
    #FIXME use different alignment for storage buffers








# Layout parsing ----------------------------------------------------------------------------------------------------------------#







# Translates a single layout
def parseLayout(glsl:str, space:bool, layouts:list, externs:list) :
    layoutInfo = re.match(
        r'layout.*?\(.*?binding=(?P<indx>[0-9]+)\)'
        r'(?P<type>buffer|uniform) (?P<iExt>ext_)?(?P<name>.*?)\{(?P<elms>.*?)\}',
        glsl
    )
    iExt = layoutInfo['iExt'] != None and len(layoutInfo['iExt']) > 0           # Check if it's external
    elms = parseElms(layoutInfo['elms'], iExt)                                  # Parse elements

    cpp = (                                                                     #Generate C++
        ('\n\n' if space else '') +                                                 # Fix spacing   # Struct declaration {
       f"\nstruct { layoutInfo['name'] }_t : public ShaderElm_b<{ 'eStorage' if layoutInfo['type'] == 'buffer' else 'eUniform' }> {{" +
       f"\n    { layoutInfo['name'] }_t() {{" + (                                       # Constructor {
       f"\n        ShaderElm_b::vdata.realloc({ str(elms['size']) });"                      # Allocate gpu data
       f"\n        ShaderElm_b:: data.realloc({ str(elms['size']) });"                      # Allocate local data copy
       f""         if not iExt else '') +                                                   # But only if the binding is not ext
       f"\n        ShaderElm_b::bind = { str(layoutInfo['indx']) };"                        # Set binding index
       f"\n    }}" + elms['func'] +                                                     # } Member access functions
       f"\n}} { layoutInfo['name'] };"                                              # } MemberDeclaration;
    )


    # If it's external, add its data to the external bidings
    if iExt: externs += [{
        'vartype' : elms['ext']['type'],
        'varname' : elms['ext']['varname'],
        'bndtype' : 'eStorage' if layoutInfo['type'] == 'buffer' else 'eUniform',
        'bndname' : layoutInfo['name']
    }]
    # Add to parsed layouts
    layouts += [{
        'type' : layoutInfo['type'],
        'name' : layoutInfo['name'],
        'indx' : layoutInfo['indx']
    }]


    return { 'cpp': cpp, 'type': layoutInfo['type'] }







def getLayouts(glsl:str):
    """!
        getLayouts Finds any layout definition, parses them and translated them to C++ structs through the parseLayout function

        @type glsl: str
        @param glsl: The raw GLSL shader code

        @rtype: dict|None
        @return: A dictionary containing the bindings, the external bindings and the generated C++ code, or None if no binding was found
            'layouts': The elements of the layout as as list of dictionaries
                'type' The type of the element
                'name' The name of the element
                'indx' The binding index of the element
            'externs': The external bindings as a list of dictionaries
                'vartype': The type of the variable #FIXME
                'varname': The name of the variable #FIXME
                'bndtype': The type of the external binding
                'bndname': The name of the external binding
            'cpp': A string containing the generated C++ code
            'storageNum': The number of storage buffers in the shader
            'uniformNum': The number of uniform buffers in the shader
    """


    glslLayouts = re.findall(r'(layout\(.*?(buffer|uniform) (.*?)\{(.*?)\};)', glsl) # TODO parse out parentheses

    if glslLayouts == None:                                         # If there are no bindings
        return None                                                     # Return None

    else:                                                           # If there is at least one binding
        layouts = []                                                    # List of elements
        externs = []                                                    # List of external bindings
        cpp = ''                                                        # Generated C++ string
        storageNum = 0; uniformNum = 0                                  # Number of storage and uniform bindings in the shader

        for i, glslLayout in enumerate(glslLayouts):                    # For each layout
            layout = parseLayout(glslLayout[0], i != 0, layouts, externs)   # Translate declaration

            if layout['type'] == 'uniform': uniformNum += 1                 # Count buffer types
            else: storageNum += 1                                           #

            cpp += indent(layout['cpp'], '\t\t')                            # Concatenate to C++ output

        return { 'cpp': cpp, 'layouts': layouts, 'externs': externs, 'storageNum': storageNum, 'uniformNum': uniformNum }







# Shader parsing ----------------------------------------------------------------------------------------------------------------#







# Parses a GLSL compute shader and writes 2 C++ interface files
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

        # Write header
        fh.write(
            f"\n//####################################################################################"
            f"\n// This file was generated by Lynx/shaders/GlslToCpp.py"
            f"\n// Changes could be overwritten without notice"
            f"\n//####################################################################################\n"
            f"\n#pragma once"                                                       # Include guard
            f"\n#include \"Lynx/Core/Render/Shaders/ShaderInterface_t.hpp\"\n\n\n"  # Base Shader struct
            f"\nnamespace lnx::shd{{"                                               # Write namespace and struct declaration
            f"\n    struct { fname } : public ShaderInterface_b {{"
            f"\n        static ShaderInterface_b::Layout layout;"
            f"\n        static uint32 pipelineIndex;"
        )

        # Write cpp
        fc.write(
            f"\n//####################################################################################"
            f"\n// This file was generated by Lynx/shaders/GlslToCpp.py"
            f"\n// Changes could be overwritten without notice"
            f"\n//####################################################################################\n"
            f"\n#include \"" + re.sub(r'^.*?\/?Lynx\/(Lynx\/.*$)', r'\g<1>', spath + shname) + f".gsi.hpp\"" #FIXME
            f"\n#include \"Lynx/Core/AutoInit.hpp\""                        # Auto init
            f"\n#include \"Lynx/Core/Render/Shaders/Shader.hpp\""           # Engine shader header
            f"\n#define LNX_H_{ fname.upper() }\n\n\n"                      # Auto init define
            f"\nnamespace lnx::shd{{"                                       # Write namespace declaration
        )




        # Expand macros
        code = subprocess.check_output(
            ['glslangValidator', spath + shname + '.comp', '-E'],
            universal_newlines=True
        )


        # Parse out unnecessary whitespace and comments from the shader code
        ncode = (
            re.sub(r'([-+])''\x07', r'\g<1> \g<1>',                         # Prevent - - and + + from being merged #! GLSL has no pointer. * * is a syntax error
            re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])( )?',  r'\g<1>',      # Remove spaces near opeartors
            re.sub(r'([-+]) \1', r'\g<1>''\x07',                            # Prevent - - and + + from being merged
            re.sub(r'\\n',      r'\n',                                      # Remove newlines
            re.sub(r'\n',       r'',                                        # Remove newlines
            re.sub(r'(#.*?)\n', r'\g<1>\\n',                                # Remove newlines
            re.sub(r' +',       r' ',                                       # Remove whitespace
            str(code).expandtabs(4)                                         # Convert tabs to spaces
        ))))))))




        pLayouts = getLayouts(ncode)

        if pLayouts != None:
            fh.write('\n\n' + pLayouts['cpp'])

            layouts = pLayouts['layouts']
            externs = pLayouts['externs']

            # Write the shader create functions
            fh.write(indent(
                #FIXME CHECK IF EXTERNS NAMES CONFLICT WITH HARD CODED FUNCTION PARAMETERS NAMES
                f"\n\n\nvoid create(" + (
                    f", ".join((f"vram::ptr<{ extern['vartype'] }, eVRam, { extern['bndtype'] }> p{ extern['varname'][0].upper() }{ extern['varname'][1:] }")
                    for extern in externs) + f", const u32v3 vGroupCount, core::RenderCore& pRenderCore);"
                ) +
                f"\nvoid createDescriptorSets();"
                f"\nvoid createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);"
                f"\nvoid updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);"
                f"\nvoid destroy();",
            '\t\t'))




            fc.write(indent(
                f"\n\n\nvoid { fname }::create(" + (
                    f", ".join((f"vram::ptr<{ extern['vartype'] }, eVRam, { extern['bndtype'] }> p{ extern['varname'][0].upper() }{ extern['varname'][1:] }")
                    for extern in externs) + f", const u32v3 vGroupCount, core::RenderCore& pRenderCore){{"
                ) +
                f"\n    pRenderCore.addObject_m.lock();" +
                    (f"".join((
                        f"\n        { extern['bndname'] }.vdata = (vram::ptr<char, eVRam, { extern['bndtype'] }>)p{ extern['varname'][0].upper() }{ extern['varname'][1:] };")
                        for extern in externs)
                    ) +
                f"\n"
                f"\n        createDescriptorSets();"
                f"\n        createCommandBuffers(vGroupCount, pRenderCore);"
                f"\n        pRenderCore.swp.shadersCBs.add(commandBuffers[0]);"
                f"\n    pRenderCore.addObject_m.unlock();"
                f"\n}}",
            '\t'))




            fc.write(indent(
                f"\n\n\nvoid { fname }::createDescriptorSets(){{"
                f"\n    vk::DescriptorPoolSize sizes[2] = {{"
                f"\n        "   f"vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount({ str(pLayouts['storageNum']) })," + (
                f"\n        " + f"vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount({ str(pLayouts['uniformNum']) })" if pLayouts['uniformNum'] > 0 else '{}' ) +
                f"\n    }};"
                f"\n    auto poolInfo = vk::DescriptorPoolCreateInfo()"
                f"\n        .setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)"
                f"\n        .setMaxSets       (1)"
                f"\n        .setPoolSizeCount ({ str(2 if pLayouts['uniformNum'] > 0 else 1) })"
                f"\n        .setPPoolSizes    (sizes)"
                f"\n    ;"
                f"\n    switch(core::dvc::graphics.ld.createDescriptorPool(&poolInfo, nullptr, &descriptorPool)){{"
                f"\n        case vk::Result::eErrorFragmentationEXT:  dbg::logError(\"Fragmentation error\");  break;"
                f"\n        vkDefaultCases;"
                f"\n    }}"
                f"\n\n\n"
                f"\n    auto allocateSetInfo = vk::DescriptorSetAllocateInfo()"
                f"\n        .setDescriptorPool     (descriptorPool)"
                f"\n        .setDescriptorSetCount (1)"
                f"\n        .setPSetLayouts        (&{ fname }::layout.descriptorSetLayout)"
                f"\n    ;"
                f"\n    switch(core::dvc::graphics.ld.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){{"
                f"\n        case vk::Result::eErrorFragmentedPool:    dbg::logError(\"Fragmented pool\");      break;"
                f"\n        case vk::Result::eErrorOutOfPoolMemory:   dbg::logError(\"Out of pool memory\");   break;"
                f"\n        vkDefaultCases;"
                f"\n    }}"
                f"\n\n\n"
                f"\n    vk::WriteDescriptorSet writeSets[{ str(len(layouts)) }];" +
                '\n'.join((
                    f"\n    auto bufferInfo{ str(i) } = vk::DescriptorBufferInfo()"
                    f"\n        .setBuffer ({ layout['name'] }.vdata.cell->csc.buffer)"
                    f"\n        .setOffset ({ layout['name'] }.vdata.cell->localOffset)"
                    f"\n        .setRange  ({ layout['name'] }.vdata.cell->cellSize)"
                    f"\n    ;"
                    f"\n    writeSets[{ str(i) }] = vk::WriteDescriptorSet()"
                    f"\n        .setDstSet          (descriptorSet)"
                    f"\n        .setDstBinding      ({ str(layout['indx']) })"
                    f"\n        .setDescriptorCount (1)"
                    f"\n        .setDescriptorType  (vk::DescriptorType::{ 'eUniformBuffer' if layout['type'] == 'uniform' else 'eStorageBuffer' })"
                    f"\n        .setPBufferInfo     (&bufferInfo{ str(i) })"
                    f"\n    ;"
                ) for i, layout in enumerate(layouts)) +
                f"\n\tcore::dvc::graphics.ld.updateDescriptorSets({ str(len(layouts)) }, writeSets, 0, nullptr);"
                f"\n}}",
            '\t'))




            fc.write(indent('\n' * 8 +
                f"\nvoid { fname }::createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){{"
                f"\n    auto allocateCbInfo = vk::CommandBufferAllocateInfo()"
                f"\n        .setCommandPool        (pRenderCore.commandPool)"
                f"\n        .setLevel              (vk::CommandBufferLevel::ePrimary)"
                f"\n        .setCommandBufferCount (1)"
                f"\n    ;"
                f"\n    commandBuffers.resize(1);"
                f"\n    switch(core::dvc::graphics.ld.allocateCommandBuffers(&allocateCbInfo, commandBuffers.begin())){{ vkDefaultCases; }}"
                f"\n"
                f"\n    auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);"
                f"\n    switch(commandBuffers[0].begin(beginInfo)){{ vkDefaultCases; }}"
                f"\n    commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[{ fname }::pipelineIndex]);"
                f"\n    commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, { fname }::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);"
                f"\n    commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);"
                f"\n    switch(commandBuffers[0].end()){{ vkDefaultCases; }}"
                #TODO WRITE ALL COMMAND BUFFERS AT ONCE
                #TODO or use multiple descriptor sets for multiple objects, but in the same command buffer
                f"\n}}",
            '\t'))




            fc.write(indent('\n' * 8 +
                f"\nvoid { fname }::updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){{"
                f"\n    auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);"
                f"\n    switch(commandBuffers[0].begin(beginInfo)){{ vkDefaultCases; }}"
                f"\n    commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[{ fname }::pipelineIndex]);"
                f"\n    commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, { fname }::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);"
                f"\n    commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);"
                f"\n    switch(commandBuffers[0].end()){{ vkDefaultCases; }}"
                f"\n}}",
            '\t'))




            fc.write(indent('\n' * 8 +
                f"\nvoid { fname }::destroy(){{"
                f"\n    //TODO"
                f"\n}}",
            '\t'))




            fc.write(indent('\n' * 8 +
                f"\nShaderInterface_b::Layout { fname }::layout;"
                f"\nuint32 { fname }::pipelineIndex = core::shaders::pipelineNum++;"
                f"\nLnxAutoInit(LNX_H_{ fname.upper() }){{"
                f"\n    core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);"
                f"\n    core::shaders::pipelineLayouts[{ fname }::pipelineIndex] = &{ fname }::layout;"
                f"\n    {{ //Create descriptor set layout"
                f"\n        vk::DescriptorSetLayoutBinding bindingLayouts[{ str(len(layouts)) }];" +
                f"\n".join((
                    f"\n        bindingLayouts[{ str(i) }] = vk::DescriptorSetLayoutBinding()"
                    f"\n            .setBinding            ({ str(layout['indx']) })"
                    f"\n            .setDescriptorType     (vk::DescriptorType::{ 'eUniformBuffer' if layout['type'] == 'uniform' else 'eStorageBuffer' })"
                    f"\n            .setDescriptorCount    (1)"
                    f"\n            .setStageFlags         (vk::ShaderStageFlagBits::eCompute)"
                    f"\n            .setPImmutableSamplers (nullptr)"
                    f"\n        ;"
                ) for i, layout in enumerate(layouts)) +
                f"\n"
                f"\n        auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()"
                f"\n            .setBindingCount ({ str(len(layouts)) })"
                f"\n            .setPBindings    (bindingLayouts)"
                f"\n        ;"
                f"\n        //Create the descriptor set layout"
                f"\n        switch(core::dvc::graphics.ld.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &{ fname }::layout.descriptorSetLayout)){{ vkDefaultCases; }}"
                f"\n    }}"
                f"\n"
                f"\n"
                f"\n"
                f"\n"
                f"\n    {{ //Create pipeline layout"
                f"\n        uint64 fileLength = 0;"
                f"\n        uint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + \"{ shname }.spv\").begin());" #TODO EVALUATE SHADER PATH AT RUNTIME
                f"\n        { fname }::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.ld, code, fileLength);"
                f"\n"
                f"\n        { fname }::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()"
                f"\n            .setStage  (vk::ShaderStageFlagBits::eCompute)"
                f"\n            .setModule ({ fname }::layout.shaderModule)"
                f"\n            .setPName  (\"main\")"
                f"\n        ;"
                f"\n"
                f"\n        auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()"
                f"\n            .setSetLayoutCount (1)"
                f"\n            .setPSetLayouts    (&{ fname }::layout.descriptorSetLayout)"
                f"\n        ;"
                f"\n        switch(core::dvc::graphics.ld.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &{ fname }::layout.pipelineLayout)){{ vkDefaultCases; }}"
                f"\n    }}"
                f"\n}}",
            '\t'))

            #TODO ADD DESTROY FUNCTION (copy from shaders::destroyShader)
            #FIXME AUTOMATIZE CPP INCLUDES
        else:
            print('No layout found. A shader must define at least one layout')

        fh.write('\n    };\n}');                              # } //Namespace
        fh.write('//TODO remove local data in external bindings') #TODO
        fc.write('\n}');                              # } //Namespace
    #TODO ADD STRUCTURE PARSING AND TRANSLATION
    #TODO STRUCTURES HAVE A MINIMUM ALIGNMENT OF 16

    return 0