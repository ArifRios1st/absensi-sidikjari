<?php

use App\Http\Middleware\DeviceTokenAuthMiddleware;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\Api;

Route::middleware([DeviceTokenAuthMiddleware::class])->name('api.v1.')->prefix('v1')->group(function () {
//    Route::apiResource('guru',Api\GuruController::class);
    Route::prefix('guru/{guru}')->name('guru.')->group(function () {
        Route::apiResource('fingerprint', Api\FingerprintController::class)->only(['index', 'update']);

    });
    Route::apiResource('absensi', Api\AbsensiController::class)->only(['index', 'store']);

    Route::prefix('device')->name('device.')->group(function () {
        Route::get('/', [Api\DeviceController::class,'index'])->name('index');
        Route::post('/', [Api\DeviceController::class,'update'])->name('update');
    });

//    Route::post('register-fingerprint/{guru}',[Api\DeviceApiController::class,'register_fingerprint'])->name('register-fingerprint');
//    Route::get('get-fingerprint-id/{guru}',[Api\DeviceApiController::class,'get_fingerprint_id'])->name('get-fingerprint-id');
//    Route::post('sync-attendance',[Api\DeviceApiController::class,'sync_attendance'])->name('sync-attendance');
//    Route::post('update-kapasitas',[Api\DeviceApiController::class,'update_kapasitas'])->name('update-kapasitas');
});
