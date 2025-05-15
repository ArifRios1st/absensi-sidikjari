<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Http\Requests\Api\StoreFingerprintRequest;
use App\Models\Fingerprint;
use App\Models\Guru;
use Illuminate\Http\Request;

class FingerprintController extends Controller
{
    public function index(Guru $guru)
    {
        return response()->json($guru->fingerprints()->get(['id', 'scanned_at']));
    }

    public function update(Request $request, Guru $guru, Fingerprint $fingerprint)
    {
        if ($fingerprint->id_guru !== $guru->id) {
            return response()->json([
                'message' => 'Fingerprint record not found for this teacher.'
            ], 404);
        }

        $fingerprint->update(['scanned_at' => now()]);

        return response()->json([
            'message' => 'Fingerprint scanned successfully.',
            'fingerprint' => [
                'id' => $fingerprint->id,
                'scanned_at' => $fingerprint->scanned_at,
            ],
        ]);
    }



}
