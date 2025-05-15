<x-filament-panels::page>

    <div class="text-center">
        <x-filament::button
            tag="a"
            href="#"
            wire:click="previousMonth"
        >
            {{ $dates['previous']  }}
        </x-filament::button>

        <x-filament::button
            tag="a"
            href="#"
            color="success"
        >
            {{ $dates['current']  }}
        </x-filament::button>

        <x-filament::button
            tag="a"
            href="#"
            wire:click="nextMonth"
        >
            {{ $dates['next']  }}
        </x-filament::button>
    </div>

    <x-filament-tables::container style="...">
        <table clas="fi-ta-table w-full table-auto divide-y divide-gray-200 text-start dark:divide-white/5">
            <thead class="divide-y divide-gray-200 dark:divide-white/5">
                <x-filament-tables::row>
                    <x-filament-tables::header-cell rowspan="2">
                        Nama
                    </x-filament-tables::header-cell>

                    <x-filament-tables::header-cell rowspan="2">
                        Kehadiran
                    </x-filament-tables::header-cell>

                    <x-filament-tables::header-cell class="text-center" colspan="{{ count($absensiGuru[array_key_first($absensiGuru)])  }}">
                        Bulan
                    </x-filament-tables::header-cell>
                </x-filament-tables::row>
                <x-filament-tables::row>
                    @foreach ($absensiGuru[array_key_first($absensiGuru)] as $date => $value)
                        <x-filament-tables::header-cell class="text-center">
                            {{ $date  }}
                        </x-filament-tables::header-cell>
                    @endforeach
                </x-filament-tables::row>
            </thead>

            <tbody class="divide-y divide-gray-200 whitespace-nowrap dark:divide-white/5">
                @foreach($absensiGuru as $guru => $dates)
                    <x-filament-tables::row>
                        <x-filament-tables::cell class="py-2 px-2" rowspan="2">
                            {{ $dataGuru[$guru]  }}
                        </x-filament-tables::cell>
                        <x-filament-tables::cell class="py-2 px-2"> Masuk</x-filament-tables::cell>
                        @foreach($dates as $date => $value)
                            <x-filament-tables::cell class="py-2 px-2 text-center">
                                {{ $value['jam_masuk'] ?? "-" }}
                            </x-filament-tables::cell>
                        @endforeach
                    </x-filament-tables::row>
                    <x-filament-tables::row>
                        <x-filament-tables::cell class="py-2 px-2">Keluar</x-filament-tables::cell>
                        @foreach($dates as $date => $value)
                            <x-filament-tables::cell class="py-2 px-2 text-center">
                                {{ $value['jam_keluar'] ?? "-" }}
                            </x-filament-tables::cell>
                        @endforeach
                    </x-filament-tables::row>
                @endforeach
            </tbody>

        </table>
    </x-filament-tables::container>

</x-filament-panels::page>
