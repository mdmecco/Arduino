(function () {
        //获取节点数组
        const getElements = (query) => {
            let ele = null;
            if (typeof query === 'string') {
                if (query.indexOf('.') > -1) {
                    ele = document.querySelectorAll(query);
                } else {
                    ele = new Array();
                    ele.push(document.querySelector(query));
                }
            }
            return ele;
        }
    
    const oldProducts = getElements('#primary-menu');
    let ele = null;

    const convertToArray = (nodes, callBackFun) => {
        let array = null, lists = [];
        array = Array.prototype.slice.call(nodes, 0);

        if (array !== null) {
            lists = callBackFun(array);        
            return lists;
        }
        return lists;
    }

    const getLists = (nodes) => {
        let lists = [];
        nodes.forEach(node => {
            if (node.nodeName.toLocaleLowerCase() === 'li') {
                lists.push(node);
            }
        });
        return lists;
    }

    if (oldProducts !== null) {
        ele = convertToArray(oldProducts[0].childNodes, getLists);
    }
    // //创建新节点
    if (ele !== null) {
        let newNode = null;
        newNode = document.createElement('li');
        newNode.className = ele[1].getAttribute('class');
        newNode.innerHTML = ele[1].innerHTML; 
        //插入节点
        const parent = getElements('#mobile-menu');
        if (newNode !== null && parent !== null) {
            newNode.setAttribute('role', 'presentation');
            parent[0].insertBefore(newNode, parent[0].childNodes[2]);
        }
    }
    
    //next
    if (document.querySelector('.price')) {
      document.querySelector('#main').style.padding = '0px';
      document.querySelector('.price').style.marginBottom = '10px';
    } 
}());
