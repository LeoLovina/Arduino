$( document ).ready(function() {
    console.log( "ready!" );
    $('#btnRealtime').click(onSetValue);

});

function onSetValue(){
    var textRef = $('#textRef');
    var jsonValue = $('#jsonValue');

    console.log(textRef.val());
    console.log(jsonValue.val());
    // get a reference to database instance 
    var database = firebase.database();

    // set json value
    var jsonObject = JSON.parse(jsonValue.val());
    database.ref(textRef.val()).set(jsonObject);
}