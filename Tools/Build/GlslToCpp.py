import os, re, sys, subprocess
from textwrap import indent
from math import ceil
#! Shaders are validated in lynxg++
#! This script is compiled with lynxg++
#TODO write what external bindings are and how to use them
#TODO add matrix support
#TODO add image support







# Element parsing ################################################################################################################








def roundUp(x : int, b : int) -> int :
    return b * ceil(x / b)






def parseElms(glsl:str) :
    """!
        Parses the elements of a layout
        Returns a tuple with a list of namespaces containing
        the type, name, array length and offset of each element, and the total size of the C++ structure
    """
    elms     : list = []
    maxAlign : int  = 0


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


#FIXME calculate array offset
    offset : int = 0
    for rInfo in re.finditer(                                    # For each binding member
        r'(?P<type>([biud]?vec[234])|(double|float|bool|(u?int))) ' # Get type name
        r'(?P<name>[a-zA-Z_]{1,}[a-zA-Z0-9_]*)'                     # Get member name
        r'(?P<iArr>\[(?P<aLen>.+?)?\])?'                            # Check if it's an array and get its length
        r';',                                                       # Anchor to instruction end
    glsl):                                                      #
        align:int = typeSize[rInfo['type']]                         # Get type alignment
        maxAlign = max(maxAlign, align)                             # Recalculate maximum alignment #TODO check if this actually works

        offset = roundUp(offset, align)                             # Recalculate element offset
        elms += [{                                                  # Save element informations
            'type': typeName[rInfo['type']],                            # Get the type and translate it to C++
            'name': rInfo['name'] ,                                     # Get the name from regex
            'aLen': None if rInfo['iArr'] == None else rInfo['aLen'],   # Save the array length or None if the element is not an array
            'ofst': offset                                              # Save the calculated offset
        }]                                                          #
        offset += align                                             # Calculate raw offset for the next element #TODO check if this actually works




    # Return a dictionary containing the translated members, the external bindings and the padded structure size
    # The size of the structure must be a multiple of 16 #BUG THE NORMAL SIZE MAKES THE ENGINE CRASH (prob buffer overflow?)
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16)) + 64})    #BUG ok
    # return dict({ 'func' : ret, 'ext' : ext, 'size' : roundUp(offset, max(maxAlign, 16))})         #BUG crash
    return {
        'elms' : elms,
        'size' : roundUp(offset, max(maxAlign, 256))
    }
    # return dict({ 'cpp' : cpp, 'types' : types, 'size' : roundUp(offset, max(maxAlign, 256)) })
    # !NVIDIA has a huge alignment of 256 bytes. #TODO use a different alignment based on the GPU, ig
    #FIXME use different alignment for storage buffers








# Layout parsing ----------------------------------------------------------------------------------------------------------------#








# Translates a single layout
def parseLayout(glsl:str) :
    rInfo = re.match(
        r'layout.*?\(.*?binding=(?P<indx>[0-9]+)\)'
        r'(?P<type>buffer|uniform) (?P<iExt>ext_)?(?P<name>.*?)\{(?P<elms>.*?)\}',
        glsl
    )

    elmsInfo = parseElms(rInfo['elms'])
    return {
        'type': 'storage' if rInfo['type'] == 'buffer' else 'uniform',
        'name': rInfo['name'],
        'iExt': rInfo['iExt'] != None and len(rInfo['iExt']) > 0,
        'indx': rInfo['indx'],
        'cstr': 'l_' + rInfo['name'],
        'elms': elmsInfo['elms'],
        'size': elmsInfo['size']
    }







def getLayouts(glsl:str):
    """!
        Finds any layout definition, parses them and translates them to C++ structs through the parseLayout function

        @type glsl: str
        @param glsl: The raw GLSL shader code

        @rtype: dict
        @return: A dictionary containing informations about the layouts
        ├─ layouts:list             A list of layouts
        │   ├─ type:str                 The type of the layout. 'storage' or 'uniform'
        │   ├─ name:str                 The name of the layout, without the external specifier
        │   ├─ iExt:bool                True if the binding is external, False otherwise
        │   ├─ indx:int                 The binding index
        │   ├─ cstr:str                 The name of the generated C++ structure
        │   ├─ elms:list                A list of elements
        │   │   ├─ type:str                 The corresponding C++ type of the element
        │   │   ├─ name:str                 The name of the element
        │   │   ├─ aLen:int                 The length of the array, or None if the element is not an array
        │   │   └─ ofst:int                 The offset of the element in the memory allocation, in bytes
        │   └─ size:int                 The size of the required allocation in bytes
        ├─ storageNum:int           The number of storage buffers used in the shader
        └─ uniformNum:int           The number of uniform buffers used in the shader
    """


    rInfo = re.findall(r'(layout\(.*?(buffer|uniform) (?:.*?)\{(?:.*?)\};)', glsl)

    layouts    : list = []                          # List of parsed layout infos
    storageNum : int  = 0                           # Number storage bindings in the shader
    uniformNum : int  = 0                           # Number of uniform bindings in the shader

    for i, rLayout in enumerate(rInfo):             # For each layout
        layouts += [ parseLayout(rLayout[0]) ]          # Translate declaration
        if rLayout[1] == 'uniform': uniformNum += 1     # Count buffer types
        else: storageNum += 1

    return {
        'layouts': layouts,
        'storageNum': storageNum,
        'uniformNum': uniformNum
    }








# File output ####################################################################################################################








def capitalize1(s:str):
    if len(s) > 1:
        return s[0].upper() + s[1:]
    else:
        return s.upper()




# Parses a GLSL compute shader and writes 2 C++ interface files
def parseShader(pathr:str, ptfm:str, rePath:str, e:bool):
    #Check files
    if not os.path.exists(pathr):
        print("File does not exist")
        return 1

    #Get shader path and name
    incShPath = os.path.dirname(os.path.relpath(pathr, rePath if e else '.'))
    shPath = os.path.dirname(os.path.relpath(pathr, '.'))
    shName = os.path.basename(pathr).rsplit('.', 1)[0]
    if re.match(r'[a-zA-Z_](\w)*', shName) == None:
        print(f"Invalid shader name \"{ shName }\". The shader name can only contain alphanumeric characters or underscores and cannot start with a digit")
        return 2



    # Expand macros
    code = subprocess.check_output(
        ['glslangValidator', shPath + '/' + shName + '.comp', '-E'],
        universal_newlines=True
    )
    # Parse out unnecessary whitespace and comments from the shader code
    ncode = (
        re.sub(r'([-+])''\x07', r'\g<1> \g<1>',                         # Prevent - - and + + from being merged #! GLSL has no pointers. * * is a syntax error
        re.sub(r' ?([()\[\]{}+*-\/.!<>=&^|?:%,;])( )?',  r'\g<1>',      # Remove spaces near opeartors
        re.sub(r'([-+]) \1', r'\g<1>''\x07',                            # Prevent - - and + + from being merged
        re.sub(r'\\n',      r'\n',                                      # Remove newlines
        re.sub(r'\n',       r'',                                        # Remove newlines
        re.sub(r'(#.*?)\n', r'\g<1>\\n',                                # Remove newlines
        re.sub(r' +',       r' ',                                       # Remove whitespace
        str(code).expandtabs(4)                                         # Convert tabs to spaces
    ))))))))




    #Parse layouts
    pGlsl = getLayouts(ncode)

    layouts = []
    externs = []
    for l in pGlsl['layouts']:
        if l['iExt']: externs += [ l ]
        else:         layouts += [ l ]








    #Write output files
    with open(shPath + '/' + shName + '.gsi.hpp', 'w') as fh, open(shPath + '/' + shName + '.gsi.cpp', 'w') as fc:


        # Write header
        fh.write(
            f"\n//####################################################################################"
            f"\n// This file was generated by Lynx/shaders/GlslToCpp.py"
            f"\n// Changes could be overwritten without notice"
            f"\n//####################################################################################\n"
            f"\n#pragma once"                                                       # Include guard
            f"\n#include \"Lynx/Core/Render/Shaders/ShaderInterface_t.hpp\"\n\n\n"  # Base Shader struct
            f"\nnamespace lnx::shd::gsi{{"                                          # Write namespace and struct declaration
            f"\n    struct { shName } : public ShaderInterface_b {{"
            f"\n        static ShaderInterface_b::Layout layout;"
            f"\n        static uint32 pipelineIndex;"
        )

        # Write cpp
        fc.write(
            f"\n//####################################################################################"
            f"\n// This file was generated by Lynx/shaders/GlslToCpp.py"
            f"\n// Changes could be overwritten without notice"
            f"\n//####################################################################################\n"
            f"\n#include \"{ incShPath + '/' + shName }.gsi.hpp\""
            f"\n#include \"Lynx/Core/AutoInit.hpp\""                        # Auto init
            f"\n#include \"Lynx/Core/Render/Shaders/Shader.hpp\""           # Engine shader header
            f"\n#include \"Lynx/Core/Render/Render.hpp\""                   # Render core definition
            f"\n#define LNX_H_{ shName.upper() }\n\n\n"                     # Auto init define
            f"\nnamespace lnx::shd::gsi{{"                                  # Write namespace declaration
        )




        # Write layout structs
        fh.write('\n\n' + indent(
            '\n\n'.join((
            f"\nstruct { l['cstr'] } : public ShaderElm_b<e{ l['type'].capitalize() }> {{"
            # External layout constructor
            f"\n    alwaysInline { l['cstr'] }(const bool vExt) {{}}"
            # Default constructor
            f"\n    inline { l['cstr'] }() {{"
            f"\n        ShaderElm_b::vdata.realloc({ l['size'] });"
            f"\n        ShaderElm_b:: data.realloc({ l['size'] });"
            f"\n    }}"
            # Copy constructor
            f"\n    inline { l['cstr'] } (const { l['cstr'] }& p{ capitalize1(l['name']) }) {{"
			f"\n    	ShaderElm_b:: data = p{ capitalize1(l['name']) }. data;"
			f"\n    	ShaderElm_b::vdata = p{ capitalize1(l['name']) }.vdata;"
			f"\n    }}"
            # Copy assignment
			f"\n    inline { l['cstr'] }& operator=(const { l['cstr'] }& p{ capitalize1(l['name']) }) {{"
			f"\n    	ShaderElm_b:: data = p{ capitalize1(l['name']) }. data;"
			f"\n    	ShaderElm_b::vdata = p{ capitalize1(l['name']) }.vdata;"
			f"\n    	return *this;"
			f"\n    	//FIXME automatically update render data after calling this function"
			f"\n    }}" +
            # Member references
            f""     ''.join((
            f"\n    { m['type'] }& { m['name'] } = *({ m['type'] }*)(ShaderElm_b::data + { m['ofst'] });"
            f""     ) for m in l['elms']) +    #! offset includes the length of the arrays  ^
            f"\n}};"
            f"\n{ l['cstr'] } { l['name'] }{{ true }};"
            f"" ) for l in pGlsl['layouts']),
        '\t\t').replace('    ', '\t'))




        # Write the shader create functions
        fh.write('\n\n' + indent(
            #FIXME CHECK IF EXTERNS NAMES CONFLICT WITH HARD CODED FUNCTION PARAMETERS NAMES
            f"\nvoid create(" + (
            f""     ','.join((
            f"\n    const { e['cstr'] }& p{ capitalize1(e['name']) }")for e in externs) + ','
            f"\n    const u32v3 vGroupCount, core::RenderCore& pRenderCore"
            f"\n);") +
            f"\nvoid createDescriptorSets();"
            f"\nvoid createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);"
            f"\nvoid updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore);"
            f"\nvoid destroy();",
        '\t\t').replace('    ', '\t'))




        fc.write('\n\n' + indent(
            f"\nvoid { shName }::create(" + (
            f""     ','.join((
            f"\n    const { e['cstr'] }& p{ capitalize1(e['name']) }" )for e in externs) + ','
            f"\n    const u32v3 vGroupCount, core::RenderCore& pRenderCore"
            f"\n){{") +
            f"\n    pRenderCore.addObject_m.lock();" + (
            f""         ''.join((f"\n\t\t{ e['name'] } = p{ capitalize1(e['name']) };") for e in externs)) +
            f"\n"
            f"\n        createDescriptorSets();"
            f"\n        createCommandBuffers(vGroupCount, pRenderCore);"
            f"\n        pRenderCore.swp.shadersCBs.add(commandBuffers[0]);"
            f"\n    pRenderCore.addObject_m.unlock();"
            f"\n}}",
        '\t').replace('    ', '\t'))




        fc.write('\n\n' + indent(
            f"\nvoid { shName }::createDescriptorSets(){{"
            f"\n    vk::DescriptorPoolSize sizes[2] = {{"
            f"\n        "   f"vk::DescriptorPoolSize().setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount({ str(pGlsl['storageNum']) })," + (
            f"\n        " + f"vk::DescriptorPoolSize().setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount({ str(pGlsl['uniformNum']) })" if pGlsl['uniformNum'] > 0 else '{}' ) +
            f"\n    }};"
            f"\n    auto poolInfo = vk::DescriptorPoolCreateInfo()"
            f"\n        .setFlags         (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)"
            f"\n        .setMaxSets       (1)"
            f"\n        .setPoolSizeCount ({ str(2 if pGlsl['uniformNum'] > 0 else 1) })"
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
            f"\n        .setPSetLayouts        (&{ shName }::layout.descriptorSetLayout)"
            f"\n    ;"
            f"\n    switch(core::dvc::graphics.ld.allocateDescriptorSets(&allocateSetInfo, &descriptorSet)){{"
            f"\n        case vk::Result::eErrorFragmentedPool:    dbg::logError(\"Fragmented pool\");      break;"
            f"\n        case vk::Result::eErrorOutOfPoolMemory:   dbg::logError(\"Out of pool memory\");   break;"
            f"\n        vkDefaultCases;"
            f"\n    }}"
            f"\n\n\n"
            f"\n    vk::WriteDescriptorSet writeSets[{ str(len(layouts)) }];" + (
            f""     '\n'.join((
            f"\n    auto bufferInfo{ str(i) } = vk::DescriptorBufferInfo()"
            f"\n        .setBuffer ({ l['name'] }.vdata.cell->csc.buffer)"
            f"\n        .setOffset ({ l['name'] }.vdata.cell->localOffset)"
            f"\n        .setRange  ({ l['name'] }.vdata.cell->cellSize)"
            f"\n    ;"
            f"\n    writeSets[{ str(i) }] = vk::WriteDescriptorSet()"
            f"\n        .setDstSet          (descriptorSet)"
            f"\n        .setDstBinding      ({ str(l['indx']) })"
            f"\n        .setDescriptorCount (1)"
            f"\n        .setDescriptorType  (vk::DescriptorType::e{ l['type'].capitalize() }Buffer)"
            f"\n        .setPBufferInfo     (&bufferInfo{ str(i) })"
            f"\n    ;"
            f""     ) for i, l in enumerate(layouts))) +
            f"\n\tcore::dvc::graphics.ld.updateDescriptorSets({ str(len(layouts)) }, writeSets, 0, nullptr);"
            f"\n}}",
        '\t').replace('    ', '\t'))




        fc.write('\n' * 8 + indent(
            f"\nvoid { shName }::createCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){{"
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
            f"\n    commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[{ shName }::pipelineIndex]);"
            f"\n    commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, { shName }::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);"
            f"\n    commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);"
            f"\n    switch(commandBuffers[0].end()){{ vkDefaultCases; }}"
            #TODO WRITE ALL COMMAND BUFFERS AT ONCE
            #TODO or use multiple descriptor sets for multiple objects, but in the same command buffer
            f"\n}}",
        '\t').replace('    ', '\t'))




        fc.write('\n' * 8 + indent(
            f"\nvoid { shName }::updateCommandBuffers(const u32v3 vGroupCount, core::RenderCore& pRenderCore){{"
            f"\n    auto beginInfo = vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);"
            f"\n    switch(commandBuffers[0].begin(beginInfo)){{ vkDefaultCases; }}"
            f"\n    commandBuffers[0].bindPipeline       (vk::PipelineBindPoint::eCompute, pRenderCore.pipelines[{ shName }::pipelineIndex]);"
            f"\n    commandBuffers[0].bindDescriptorSets (vk::PipelineBindPoint::eCompute, { shName }::layout.pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);"
            f"\n    commandBuffers[0].dispatch           (vGroupCount.x, vGroupCount.y, vGroupCount.z);"
            f"\n    switch(commandBuffers[0].end()){{ vkDefaultCases; }}"
            f"\n}}",
        '\t').replace('    ', '\t'))




        fc.write('\n' * 8 + indent(
            f"\nvoid { shName }::destroy(){{"
            f"\n    //TODO"
            f"\n}}",
        '\t').replace('    ', '\t'))




        fc.write('\n' * 8 + indent(
            f"\nShaderInterface_b::Layout { shName }::layout;"
            f"\nuint32 { shName }::pipelineIndex = core::shaders::pipelineNum++;"
            f"\nLnxAutoInit(LNX_H_{ shName.upper() }){{"
            f"\n    core::shaders::pipelineLayouts.resize(core::shaders::pipelineNum);"
            f"\n    core::shaders::pipelineLayouts[{ shName }::pipelineIndex] = &{ shName }::layout;"
            f"\n    {{ //Create descriptor set layout"
            f"\n        vk::DescriptorSetLayoutBinding bindingLayouts[{ str(len(layouts)) }];" + (
            f""         '\n'.join((
            f"\n        bindingLayouts[{ str(i) }] = vk::DescriptorSetLayoutBinding()"
            f"\n            .setBinding            ({ str(l['indx']) })"
            f"\n            .setDescriptorType     (vk::DescriptorType::{ 'eUniformBuffer' if l['type'] == 'uniform' else 'eStorageBuffer' })"
            f"\n            .setDescriptorCount    (1)"
            f"\n            .setStageFlags         (vk::ShaderStageFlagBits::eCompute)"
            f"\n            .setPImmutableSamplers (nullptr)"
            f"\n        ;"
            f""         ) for i, l in enumerate(layouts))) +
            f"\n"
            f"\n        auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo()"
            f"\n            .setBindingCount ({ str(len(layouts)) })"
            f"\n            .setPBindings    (bindingLayouts)"
            f"\n        ;"
            f"\n        //Create the descriptor set layout"
            f"\n        switch(core::dvc::graphics.ld.createDescriptorSetLayout(&layoutCreateInfo, nullptr, &{ shName }::layout.descriptorSetLayout)){{ vkDefaultCases; }}"
            f"\n    }}"
            f"\n"
            f"\n"
            f"\n"
            f"\n"
            f"\n    {{ //Create pipeline layout"
            f"\n        uint64 fileLength = 0;"
            f"\n        uint32* code = core::shaders::loadSpv(&fileLength, (core::shaders::shaderPath + \"{ shName }.spv\").begin());" #TODO EVALUATE SHADER PATH AT RUNTIME
            f"\n        { shName }::layout.shaderModule = core::shaders::createModule(core::dvc::graphics.ld, code, fileLength);"
            f"\n"
            f"\n        { shName }::layout.shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()"
            f"\n            .setStage  (vk::ShaderStageFlagBits::eCompute)"
            f"\n            .setModule ({ shName }::layout.shaderModule)"
            f"\n            .setPName  (\"main\")"
            f"\n        ;"
            f"\n"
            f"\n        auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()"
            f"\n            .setSetLayoutCount (1)"
            f"\n            .setPSetLayouts    (&{ shName }::layout.descriptorSetLayout)"
            f"\n        ;"
            f"\n        switch(core::dvc::graphics.ld.createPipelineLayout(&pipelineLayoutCreateInfo, nullptr, &{ shName }::layout.pipelineLayout)){{ vkDefaultCases; }}"
            f"\n    }}"
            f"\n}}",
        '\t').replace('    ', '\t'))

        #TODO ADD DESTROY FUNCTION (copy from shaders::destroyShader)
        #FIXME AUTOMATIZE CPP INCLUDES

        fh.write('\n    };\n}');    # }; //Struct } //Namespace
        fc.write('\n}');            # } //Namespace
    #TODO STRUCTURES HAVE A MINIMUM ALIGNMENT OF 16

    return 0