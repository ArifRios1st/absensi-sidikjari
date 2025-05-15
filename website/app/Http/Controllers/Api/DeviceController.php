<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Http\Requests\Api\UpdateDeviceRequest;
use App\Models\Device;
use Illuminate\Http\Request;

class DeviceController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        return response()->json(Device::first());
    }

    /**
     * Update the specified resource in storage.
     */
    public function update(UpdateDeviceRequest $request)
    {
        Device::first()->update($request->validated());

        return response()->json(['message' => 'Device updated successfully.']);
    }
}
