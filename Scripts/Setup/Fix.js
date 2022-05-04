

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}





var setup_fix = {
    // Remove HTML indentation from pre blocks and add the line number
    format : function(s){
        // Find shortest indentation
        let min;
        s = s.split('\n');
        for(let i = 0; i < s.length; i++){
            let line = s[i];
            let spaces = line.search(/\S/);
            if((min > spaces || min == undefined) && spaces >= 0){
                min = spaces;
            }
        }
        // Remove indentation
        for(let i = 0; i < s.length; i++){
            let line = s[i];
            s[i] = (i == 0 || i == s.length - 1) ? '' : '<span class="hidden">' +(('0' + i).slice(-2)) + '&nbsp&nbsp;</span>';
            s[i] += line.substring(min, line.length);
        }

        // Join and return all lines
        return s.join('\n');
    },




    // Fix code indentation because apparently HTML5+CSS3 can't do that
    format_code : function() {
        let children = document.getElementsByTagName('EXAMPLE-');
        for(let i = 0; i < children.length; i++){
            let children2 = children[i].getElementsByTagName('SCROLL-');
            for(let j = 0; j < children2.length; ++j){
                children2[j].innerHTML = setup_fix.format(children2[j].innerHTML) + '\n';
                // Add trailing newline to fix CSS's bugged max-height  ^
            }
        }
    },




    // Add 0 width <sup> and <sub> elements to <elm-> tags as they usually have a <sup> tag inside and it screws up the vertical alignment
    fix_elm_height : function(){
        let children = document.getElementsByTagName('ELM-');
        for(let i = 0; i < children.length; i++){
            children[i].innerHTML +=
                '<sup class="nobefore noafter" style="display: inline; margin-left: -100%;"></sup>' +
                '<sub class="nobefore noafter" style="display: inline; margin-left: 100%;"></sub>'
            ;
        }
    },




    init : function() {
        setup_fix.format_code();
        setup_fix.fix_elm_height();
    }
}