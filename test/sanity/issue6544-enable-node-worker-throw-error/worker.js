onmessage = (event) => {
    console.log('I will try to throw an error in 2 seconds');

    setTimeout(function () {
        throw new Error('Normal Error');
    }, 2000);
};