// ======================================
function create_obj() {
    td = navigator.appName;
    if (td == "Microsoft Internet Explorer") {
        obj = new ActiveXObject("Microsoft.XMLHTTP");
    } else {
        obj = new XMLHttpRequest();
    }
    return obj;
}
var xhttp = create_obj();
//-----------Thiết lập dữ liệu và gửi request-------
function getdata(url) {
    xhttp.open("GET", "/" + url, true);
    xhttp.onreadystatechange = process; //nhận reponse 
    xhttp.send();
}
//-----------Kiểm tra response--------------------
function process() {
    if (xhttp.readyState == 4 && xhttp.status == 200) {
        //------Updat data sử dụng javascript----------
        ketqua = xhttp.responseText;

    }
}
const tempPhongKhach = document.querySelector('.tempPhongKhach');
const humPhongKhach = document.querySelector('.humPhongKhach');
const tempPhongNgu = document.querySelector('.tempPhongNgu');
const humPhongNgu = document.querySelector('.humPhongNgu');
const gasPhongBep = document.querySelector('.gasPhongBep');
const dataPhongKhach = {
    node: 1,
    temp: 99,
    hum: 99,
};

const dataPhongNgu = {
    node: 2,
    temp: 99,
    hum: 99,
};

const dataPhongBep = {
    node: 3,
    gas: 99,
};

// function collectSensor() {
//     xhttp.open("GET", "upload", true); // trùng với url xx/upload
//     xhttp.onreadystatechange = process_json;
//     xhttp.send();
// }

// function collectClientSensor() {
//     xhttp.open("GET", "upload1", true);
//     xhttp.onreadystatechange = process_json1;
//     xhttp.send();
// }

const updateData = () => {
    tempPhongKhach.innerText = dataPhongKhach.temp.toFixed(1) + '\°C';
    humPhongKhach.innerText = dataPhongKhach.hum + '%';
    tempPhongNgu.innerText = dataPhongNgu.temp.toFixed(1) + '\°C';
    humPhongNgu.innerText = dataPhongNgu.hum + '%';
    gasPhongBep.innerText = dataPhongBep.gas + ' ppm';

};

// function process_json() {
//     if (xhttp.readyState == 4 && xhttp.status == 200) {
//         //------Update data sử dụng javascript-------------------
//         var data = xhttp.responseText;
//         var Obj = JSON.parse(data);
//         dataPhongKhach.temp = Obj.temp;
//         dataPhongKhach.hum = Obj.hum;
//     }
// }

// function process_json1() {
//     if (xhttp.readyState == 4 && xhttp.status == 200) {
//         //------Update data sử dụng javascript-------------------
//         var data = xhttp.responseText;
//         var Obj = JSON.parse(data);
//         var tempNode = Obj.node;
//         if (tempNode == 2) {
//             dataPhongNgu.temp = Obj.temp;
//             dataPhongNgu.hum = Obj.hum;
//         } else if (tempNode == 3) {
//             dataPhongBep.gas = Obj.gas;
//         }

//     }
// }


setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = xhttp.responseText;
            var Obj = JSON.parse(data);
            dataPhongNgu.temp = Obj.temp;
             dataPhongNgu.hum = Obj.hum;
        }
    };
    xhttp.open("GET", "/upload", true);
    xhttp.send();
}, 1500);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = xhttp.responseText;
            var Obj = JSON.parse(data);
            var tempNode = Obj.node;
            if (tempNode == 1) {
 	      dataPhongKhach.temp = Obj.temp;
               dataPhongKhach.hum = Obj.hum;
            } else if (tempNode == 3) {
                dataPhongBep.gas = Obj.gas;
            }
        }
    };
    xhttp.open("GET", "/upload1", true);
    xhttp.send();
}, 2500);

setInterval(() => {
    updateData();
}, 500); //request sau 2s