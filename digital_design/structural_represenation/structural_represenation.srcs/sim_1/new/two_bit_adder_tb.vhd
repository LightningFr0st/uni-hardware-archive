----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/22/2025 09:02:47 PM
-- Design Name: 
-- Module Name: two_bit_adder_tb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity two_bit_adder_tb is
--  Port ( );
end two_bit_adder_tb;

architecture Behavioral of two_bit_adder_tb is

component two_bit_adder_beh  
Port (
    A    : in  STD_LOGIC_VECTOR(1 downto 0);
    B    : in  STD_LOGIC_VECTOR(1 downto 0);
    Cin  : in  STD_LOGIC;
    S    : out STD_LOGIC_VECTOR(1 downto 0);
    Cout : out STD_LOGIC
);
end component;

component two_bit_adder_struct  
Port (
    A    : in  STD_LOGIC_VECTOR(1 downto 0);
    B    : in  STD_LOGIC_VECTOR(1 downto 0);
    Cin  : in  STD_LOGIC;
    S    : out STD_LOGIC_VECTOR(1 downto 0);
    Cout : out STD_LOGIC
);
end component;


signal test_vector : std_logic_vector (4 downto 0);
signal a_vector : std_logic_vector(1 downto 0);
signal b_vector : std_logic_vector(1 downto 0);
signal c_in : std_logic;

signal sum_struct : std_logic_vector(1 downto 0);
signal sum_beh : std_logic_vector(1 downto 0);

signal cout_struct : std_logic;
signal cout_beh : std_logic;

signal error : std_logic;

constant period : time := 25 ns;

begin
     
     beh_adder: two_bit_adder_beh port map(
        a => a_vector,
        b => b_vector,
        cin => c_in,
        S => sum_beh,
        Cout => cout_beh
     );
     
     beh_struct: two_bit_adder_struct port map(
        a => a_vector,
        b => b_vector,
        cin => c_in,
        S => sum_struct,
        Cout => cout_struct
     );
     
     a_vector <= test_vector(4 downto 3);
     b_vector <= test_vector(2 downto 1);
     c_in <= test_vector(0);
     
     error <= '1' when (sum_struct /= sum_beh) or (cout_struct /= cout_beh) else '0';
     
     tb_proc : process
        variable error_count : integer := 0;
     begin
        
        report "Starting testbench...";
        
        for i in 0 to 31 loop
            test_vector <= std_logic_vector(to_unsigned(i, test_vector'length));
            wait for period;
            
            if error = '1' then
                error_count := error_count + 1;
                report "ERROR at test vector: " & integer'image(i);
            end if;
        end loop;
        
        if error_count = 0 then
            report "ALL TESTS PASSED! Both implementations are equivalent." severity note;
        else
            report "TEST FAILED" severity note;
        end if;
        
        report "End of simulation" severity failure;
     
     end process;
     
end Behavioral;
