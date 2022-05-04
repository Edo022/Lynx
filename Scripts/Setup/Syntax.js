

var setup_syntax = {
    // Fix syntax codes having different heights because any CSS solution doesn't work and i don't want to add a million wrapper tags
    even_heights : function(){
        let children = document.getElementsByTagName('LEFT-');
        for(let i = 0; i < children.length; i++){
            let left = children[i];
            if(left.parentNode.tagName == 'SYNTAX-' || left.parentNode.tagName == 'EXAMPLE-') {
                let right;
                let children2 = left.parentNode.children;
                for(let j = 0; j < children2.length; j++){
                    if(children2[j].tagName == 'RIGHT-') {
                        right = children2[j];
                        break;
                    }
                }
                let max = Math.max(left.clientHeight, right.clientHeight);
                left.style.height  = `${ max }px`;
                right.style.height = `${ max }px`;
            }
        }
    },



    // Create syntax path arrows with svg elements
    format_arrows : function(){
        // Calculate constants
        let margin = parseFloat(window.getComputedStyle(document.documentElement).getPropertyValue('--syntax-arrow-margin').slice(0, -2));
        let arrow_d = 5;
        let r = 0.05, fix = 0.04;


        // Get children
        children = Array();
        let tags = [
            'TR',  'TB',  'TL',  'RB',  'RL',  'RT',  'BL',  'BT',  'BR',  'LT',  'LR',  'LB',
            'TRC', 'TBC', 'TLC', 'RBC', 'RLC', 'RTC', 'BLC', 'BTC', 'BRC', 'LTC', 'LRC', 'LBC',
            'C'
        ];
        for(let i = 0; i < tags.length; ++i) {
            children = children.concat(Array.from(document.getElementsByTagName(`B-${ tags[i] }-`)));
        }


        // For each child
        for(let i = 0; i < children.length; ++i) {
            let tagName = children[i].tagName;
            let w = children[i].parentElement.clientWidth - margin * 2;
            let h = children[i].clientHeight;
            let s = '';


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
                let a = Array('L'.charCodeAt(0));
                    a['T'.charCodeAt(0)] = Math.PI * 1.5; //! y is inverted
                    a['R'.charCodeAt(0)] = Math.PI * 0.0;
                    a['B'.charCodeAt(0)] = Math.PI * 0.5; //! y is inverted
                    a['L'.charCodeAt(0)] = Math.PI * 1.0;
                let
                    ax0 = Math.cos(a[tagName.charCodeAt(2)]) * (1 + fix), ax = ax0 / 2 + 0.5,
                    ay0 = Math.sin(a[tagName.charCodeAt(2)]) * (1 + fix), ay = ay0 / 2 + 0.5,
                    bx0 = Math.cos(a[tagName.charCodeAt(3)]) * (1 + fix), bx = bx0 / 2 + 0.5,
                    by0 = Math.sin(a[tagName.charCodeAt(3)]) * (1 + fix), by = by0 / 2 + 0.5,
                    h0x = Math.cos(a[tagName.charCodeAt(3)] + Math.PI * 1.25),
                    h0y = Math.sin(a[tagName.charCodeAt(3)] + Math.PI * 1.25),
                    h1x = Math.cos(a[tagName.charCodeAt(3)] - Math.PI * 1.25),
                    h1y = Math.sin(a[tagName.charCodeAt(3)] - Math.PI * 1.25)
                ;

                // Draw arrow
                s =
                    `M${ ax * w },${ ay * h }` +                                                    // Starting position
                    `L${ (ax - ax0 * r) * w },${ (ay - ay0 * r) * h }` +                            // Draw starting segment
                    `Q${ 0.5 * w },${ 0.5 * h } ${ (bx - bx0 * r) * w },${ (by - by0 * r) * h }` +  // Draw curve
                    `L${ bx * w },${ by * h }`                                                      // Draw ending segment
                ;
                if(tagName[4] != 'C') s +=
                    `m${ h0x * arrow_d },${ h0y * arrow_d }` +                                      // Starting position
                    `L${ bx * w },${ by * h }` +                                                    // Draw head form 0 to O
                    `l${ h1x * arrow_d },${ h1y * arrow_d }`                                        // Draw head from O to 1
                ;
            }


            // Add HTML boilerplate
            children[i].innerHTML =
                `<svg width="${ w }" height="${ h }">` +
                    `<path d="${ s }"style="stroke: var(--fg-arrow); stroke-width: 2px; fill: none;"/>` +
                `</svg>`
            ;
        }




        // Wrap lines in divs to control the line height
        // The div is styled from CSS
        children = document.getElementsByTagName('SYNTAX-');
        for(let i = 0; i < children.length; ++i) {
            let children2 = children[i].getElementsByTagName('TD');

            for(let j = 0; j < children2.length; ++j){
                var is_end = children2[j].getElementsByTagName('B-E-').length;
                children2[j].innerHTML =
                    `<div${ is_end ? ' style="min-height:0;max-height:0;"' : '' }>` +
                        children2[j].innerHTML +
                    '</div>'
                ;
                // Set td line height
                if(is_end) children2[j].parentElement.style.lineHeight = '0';
            }
        }
    },








    init : function() {
        setup_syntax.format_arrows();
        setup_syntax.even_heights();
    }
}