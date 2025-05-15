<?php

namespace App\Http\Controllers\Api;

use App\Enums\HariEnum;
use App\Enums\SyncAbsensiStatusEnum;
use App\Http\Controllers\Controller;
use App\Http\Requests\Api\StoreAbsensiRequest;
use App\Models\Absensi;
use App\Models\Fingerprint;
use App\Models\Jadwal;
use Carbon\Carbon;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;

class AbsensiController extends Controller
{
    public function index()
    {
        //
    }

    public function store(StoreAbsensiRequest $request)
    {
        try {
            DB::beginTransaction(); // Start transaction

            $attendances = $request->validated()['attendances'];

            $result = [];

            foreach ($attendances as $attendance) {
                $fingerprint = Fingerprint::where('id', $attendance['fingerprint_id'])->first();

                if (!$fingerprint) {
                    $result[] = (object) ["id" => $attendance['id'], "status" => SyncAbsensiStatusEnum::DELETED];
                    continue;
                }

                $timestamp = Carbon::parse($attendance['timestamp']);
                $tanggal = $timestamp->toDateString();
                $timeOnly = $timestamp->toTimeString();
                $dayEnum = HariEnum::tryFrom(strtoupper($timestamp->translatedFormat('l')));

                if (!$dayEnum) {
                    throw new \Exception("Invalid day for timestamp: {$attendance['timestamp']}");
                }

                $jadwal = Jadwal::where('hari', $dayEnum->value)->first();

                if (!$jadwal) {
                    $result[] = (object) ["id" => $attendance['id'], "status" => SyncAbsensiStatusEnum::DELETED];
                    continue;
                }

                // Fetch today's attendance for the guru
                $absensi = Absensi::where('id_guru', $fingerprint->id_guru)->where('tanggal', $tanggal)->first();

                if (!$absensi) {
                    // No attendance record -> create new one (First scan)
                    Absensi::create([
                        'id_guru' => $fingerprint->id_guru,
                        'tanggal' => $tanggal,
                        'jam_masuk' => $timeOnly,
                        'jadwal_masuk' => $jadwal->jam_masuk,
                        'jadwal_keluar' => $jadwal->jam_keluar,
                        'toleransi_keterlambatan' => $jadwal->toleransi_keterlambatan,
                    ]);
                } else {
                    // Attendance exists -> check jam_masuk and jam_keluar
                    if (!$absensi->jam_masuk) {
                        // If jam_masuk is empty, fill it
                        $absensi->update(['jam_masuk' => $timeOnly]);
                    } elseif (!$absensi->jam_keluar) {
                        // Ensure guru has already checked in
                        $jamMasuk = Carbon::parse($absensi->jam_masuk);
                        $jadwalKeluar = Carbon::parse($absensi->jadwal_keluar);

                        // Check if scanned time is >= jadwal_keluar
                        if ($timestamp->greaterThanOrEqualTo($jadwalKeluar)) {
                            $absensi->update(['jam_keluar' => $timeOnly]);
                        }
                    }
                }

                $result[] = (object) ["id" => $attendance['id'], "status" => SyncAbsensiStatusEnum::OK];
            }

            DB::commit(); // Commit transaction
            return response()->json($result);
        } catch (\Exception $e) {
            DB::rollBack(); // Rollback on error
            return response()->json(['error' => $e->getMessage()], 400);
        }
    }
}
