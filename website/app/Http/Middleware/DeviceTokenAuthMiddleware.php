<?php

namespace App\Http\Middleware;

use App\Models\Device;
use Closure;
use Illuminate\Http\Request;
use Symfony\Component\HttpFoundation\Response;

class DeviceTokenAuthMiddleware
{
    /**
     * Handle an incoming request.
     *
     * @param Closure(Request): (Response) $next
     */
    public function handle(Request $request, Closure $next): Response
    {
        $token = $request->bearerToken();

        // Fetch the device if it exists
        $device = Device::where('token', $token)->first();

        if (!$device) {
            return response()->json(['message' => 'Unauthorized'], 401);
        }

        $request->attributes->set('device', $device);

        return $next($request);
    }
}
