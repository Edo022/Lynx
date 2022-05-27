

var setup_syntax = {
    // Align the colums to the maximum width of their cells
    even_widths : function(){
        let tables = document.querySelectorAll('syntax2- > table');
        for(let j = 0; j < tables.length; ++j){
            let trs = tables[j].querySelectorAll('tr');

            // Calculate max width
            let max = Array();
            for(let k = 0; k < trs.length; ++k) {

                let tds = trs[k].querySelectorAll('td');
                for(let l = 0; l < tds.length; ++l){
                    if(Object.is(max[l], undefined)) {
                        max[l] = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-w').slice(0, -2));
                    }
                    if(max[l] < tds[l].clientWidth) max[l] = tds[l].clientWidth;
                }
            }

            // Set max width
            for(let k = 0; k < trs.length; ++k) {
                let tds = trs[k].querySelectorAll('td');
                for(let l = 0; l < tds.length; ++l){
                    tds[l].style.width = `${ max[l] }px`;

                    // Also add a wrapper div, cause why not?
                    // if(tds[l].get)
                    let e = tds[l].childNodes;
                    if(e.length) e[0].innerHTML =
                        '<div>' +
                            e[0].innerHTML +
                        '</div>'
                    ;
                }
            }
        }
    },




    // Create syntax path arrows with svg elements
    format_arrows : function(){
        // Calculate constants
        let margin = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-margin').slice(0, -2));
        let arrow_d = 5;                                                                                                 //!^ Remove 'px' suffix
        let fix = 0.04, mfix = 0.3, r = 0.2;

        let a = Array();
            a['T'.charCodeAt(0)] = Math.PI * 1.5; //! T is inverted
            a['R'.charCodeAt(0)] = Math.PI * 0.0;
            a['B'.charCodeAt(0)] = Math.PI * 0.5; //! T is inverted
            a['L'.charCodeAt(0)] = Math.PI * 1.0;
        ;


        // For each tag
        let children = document.querySelectorAll(
            'B-TR-,   B-TB-,   B-TL-,   B-RB-,   B-RL-,  B-RT-,  B-BL-,  B-BT-,  B-BR-,  B-LT-,  B-LR-,  B-LB-, '  +
            'B-TRC-,  B-TBC-,  B-TLC- , B-RBC-,  B-RLC-, B-RTC-, B-BLC-, B-BTC-, B-BRC-, B-LTC-, B-LRC-, B-LBC-, ' +
            'B-TR-B-, B-BR-B-, B-LT-B-, B-LR-B-, B-LB-B-, ' +
            'B-TRCB-, B-BRCB-, B-LTCB-, B-LRCB-, B-LBCB-, ' +
            'B-TR-T-, B-BR-T-, B-LT-T-, B-LR-T-, B-LB-T-, ' +
            'B-TRCT-, B-BRCT-, B-LTCT-, B-LRCT-, B-LBCT-, ' +
            'B-C-'
        );
        for(let j = 0; j < children.length; ++j) {
            let tagName = children[j].tagName;
            let w = children[j].parentElement.clientWidth - margin * 2;
            let h = children[j].clientHeight;
            let s = '';
            let f = tagName.charCodeAt(2);
            let t = tagName.charCodeAt(3);
            let m = 0; switch(tagName[5]) { case 'T': m = -1; break; case 'B': m = 1; }; //! Y is inverted
            let ma = (m && (tagName[2] == 'R' || tagName[2] == 'L')) ? (mfix * m) : 0;
            let mb = (m && (tagName[3] == 'R' || tagName[3] == 'L')) ? (mfix * m) : 0;
            children[j].className += "syntax-arrow";


            // Connector
            if(tagName[2] == 'C') {
                s =
                    `M${ w * (1 - fix) },${ 0.5 * h }` +
                    `l${ (margin + w * fix) * 2 },0`
                ;
            }


            // Arrows
            else {
                // Calculate coordinates
                // let r2 = r + mfix * Math.abs(m);
                let r2 = r;
                let
                    ax = (Math.cos(a[f]) * (1 + fix     ))      / 2 + 0.5,
                    aX = (Math.cos(a[f]) * (1 + fix - r2))      / 2 + 0.5,
                    ay = (Math.sin(a[f]) * (1 + fix     ) + ma) / 2 + 0.5,
                    aY = (Math.sin(a[f]) * (1 + fix - r2) + ma) / 2 + 0.5,
                    bx = (Math.cos(a[t]) * (1 + fix     ))      / 2 + 0.5,
                    bX = (Math.cos(a[t]) * (1 + fix - r2))      / 2 + 0.5,
                    by = (Math.sin(a[t]) * (1 + fix     ) + mb) / 2 + 0.5,
                    bY = (Math.sin(a[t]) * (1 + fix - r2) + mb) / 2 + 0.5,

                    cx = 0 / 2 + 0.5,
                    cy = (ma ? ma : mb) / 2 + 0.5,

                    h0x = Math.cos(a[t] + Math.PI * 1.25),
                    h0y = Math.sin(a[t] + Math.PI * 1.25),
                    h1x = Math.cos(a[t] - Math.PI * 1.25),
                    h1y = Math.sin(a[t] - Math.PI * 1.25)
                ;

                // Draw arrow
                s =
                    `M${ ax * w },${ ay * h }` +                            // Starting position
                    `L${ aX * w },${ aY * h }` +                            // Draw starting segment
                    `Q${ cx * w },${ cy * h } ${ bX * w },${ bY * h }` +    // Draw curve
                    `L${ bx * w },${ by * h }`                              // Draw ending segment
                ;
                if(tagName[4] != 'C') s +=
                    `m${ h0x * arrow_d },${ h0y * arrow_d }` +              // Starting position
                    `L${ bx * w },${ by * h }` +                            // Draw head form 0 to O
                    `l${ h1x * arrow_d },${ h1y * arrow_d }`                // Draw head from O to 1
                ;
            }


            // Add HTML boilerplate
            children[j].innerHTML =
                `<svg width="${ w }" height="${ h }">` +
                    `<path d="${ s }"style="stroke: var(--syntax-fg-arrow); stroke-width: 2px; fill: none;"/>` +
                `</svg>`
            ;
        }
    },







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
        return s.join('<br>');
    },



    // Fix code indentation because apparently HTML5+CSS3 can't do that
    format_code : function() {
        let c = document.querySelectorAll('example2-');
        for(let i = 0; i < c.length; i++){
            let divs = c[i].querySelectorAll('div');
            for(let j = 0; j < divs.length; ++j){
                divs[j].innerHTML = setup_syntax.format(divs[j].innerHTML) + '<br>';
                // Add trailing newline to fix CSS's bugged max-height  ^
            }
        }
    },







    init : function() {
        setup_syntax.even_widths();
        setup_syntax.format_arrows();
        setup_syntax.format_code();
    }
}