<?php

namespace Database\Seeders;

use App\Enums\HariEnum;
use App\Models\Jadwal;
use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;

class JadwalSeeder extends Seeder
{
    /**
     * Run the database seeds.
     */
    public function run(): void
    {
        Jadwal::factory()->create([
            'hari' => HariEnum::SENIN
        ]);
        Jadwal::factory()->create([
            'hari' => HariEnum::SELASA
        ]);
        Jadwal::factory()->create([
            'hari' => HariEnum::RABU
        ]);
        Jadwal::factory()->create([
            'hari' => HariEnum::KAMIS
        ]);
        Jadwal::factory()->create([
            'hari' => HariEnum::JUMAT
        ]);
        Jadwal::factory()->create([
            'hari' => HariEnum::SABTU
        ]);
    }
}
